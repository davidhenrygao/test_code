#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define MAX_BUF_LEN 256

//Because file descriptor fd 0 reference to the kernel's stdin file descrition,
//and the stdin file descrition in the kernel is only referenced by this program,
//so when close fd 0 in this program, the stdin file descrition in kernel will 
//not be closed.

//But why the epoll_wait in level trigger will always return EPOLLIN events?
//Because when you close fd 0, the epoll will get a EPOLLIN event which will 
//return 0 when read the fd, but the fd has been closed, so read fails and 
//epoll in level trigger mode will return it next time.

int main(int argc, char *argv[]) {
  int efd;
  struct epoll_event ee, ees[MAX_EVENTS];
  int nevents;
  ssize_t rb;
  char rbuf[MAX_BUF_LEN];
  int i;
  int counter = 0;

  efd = epoll_create(10);
  //ee.events = EPOLLIN | EPOLLET;  //test using ET mode.
  ee.events = EPOLLIN;
  ee.data.fd = STDIN_FILENO;
  epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &ee);

  printf("main(read) thread wait read events.\n");
  while (1) {
    nevents = epoll_wait(efd, ees, MAX_EVENTS, -1);
    if (nevents == -1) {
      printf("epoll_wait error: %s.\n", strerror(errno));
      break;
    }
    counter++;
    printf("%dth epoll_wait return.\n", counter);
    for (i = 0; i < nevents; ++i) {
      if (ees[i].events | EPOLLIN) {
        rb = read(ees[i].data.fd, rbuf, MAX_BUF_LEN);
        if (rb < 0) {
          printf("read error on fd(%d): %s\n", ees[i].data.fd, strerror(errno));
        }
        if (rb == 0) {
          printf("write end close.\n");
          close(ees[i].data.fd);
        }
        if (rb > 0) {
          printf("read data: %s.\n", rbuf);
        }
      } else if (ees[i].events | EPOLLERR) {
        printf("epoll error on fd(%d).\n", ees[i].data.fd);
      }
    }
    if (counter == 1) {
      close(STDIN_FILENO);
    }
    if (counter > 10) {
      break;
    }
  }

  printf("main(read) thread exit.\n");
  return 0;
}
