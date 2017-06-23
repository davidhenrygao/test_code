#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

/*
 * 1.compile the source code directly and test;
 * 2.uncommented the test1 lines, compile and test;
 * 3.uncommented the test2 lines, compile and test;
 * 4.uncommented the test1 and test2 lines, compile and test;
 */

#define MAX_ERR_BUF_LEN 128
char err_rbuf[MAX_ERR_BUF_LEN];
char err_wbuf[MAX_ERR_BUF_LEN];

char *Strerror(int err, char *buf, size_t len) {
  strerror_r(err, buf, len);
  return buf;
}

void *writer_thread(void* arg) {
  int i;
  ssize_t wb;
  int wfd = *(int*)arg;
  printf("writer thread run and sleep 1 sec.\n");
  sleep(1);
  for (i = 1; i <= 5; ++i) {
    printf("writer thread write %d.\n", i);
    wb = write(wfd, &i, sizeof(i));
    if (wb < 0) {
      printf("write error: %s.\n", 
          Strerror(errno, err_wbuf, MAX_ERR_BUF_LEN));
    }
    sleep(1);
  }
  printf("writer thread exit.\n");
  return NULL;
}

int main(int argc, char *argv[]) {
  int pipefds[2];
  int efd;
  struct epoll_event ee, ees[MAX_EVENTS];
  pthread_t tid;
  int counter;
  int nevents;
  int result;
  ssize_t rb;
  int i;
  //int dupfd; //test1: duplicate fd

  pipe(pipefds);
  //dupfd = dup(pipefds[0]);  //test1

  efd = epoll_create(10);
  //ee.events = EPOLLIN | EPOLLET;  //test2: using ET mode.
  ee.events = EPOLLIN;
  ee.data.fd = pipefds[0];
  epoll_ctl(efd, EPOLL_CTL_ADD, pipefds[0], &ee);

  //ignore sigpipe
  signal(SIGPIPE, SIG_IGN);

  pthread_create(&tid, NULL, &writer_thread, (void*)&pipefds[1]);

  printf("main(read) thread wait read events.\n");
  counter = 0;
  while (1) {
    nevents = epoll_wait(efd, ees, MAX_EVENTS, 2000);
    if (nevents == -1) {
      printf("epoll_wait error: %s.\n", 
          Strerror(errno, err_rbuf, MAX_ERR_BUF_LEN));
      break;
    }
    counter++;
    printf("%dth epoll_wait return.\n", counter);
    for (i = 0; i < nevents; ++i) {
      if (ees[i].events | EPOLLIN) {
        rb = read(ees[i].data.fd, &result, sizeof(result));
        if (rb < 0) {
          printf("read error on fd(%d): %s\n", ees[i].data.fd, 
              Strerror(errno, err_rbuf, MAX_ERR_BUF_LEN));
        }
        if (rb == 0) {
          printf("write end close.\n");
          close(ees[i].data.fd);
        }
        if (rb > 0) {
          printf("read data: %d.\n", result);
        }
      } else if (ees[i].events | EPOLLERR) {
        printf("epoll error on fd(%d).\n", ees[i].data.fd);
      }
    }
    if (counter == 3) {
      close(pipefds[0]);
    }
    if (counter > 10) {
      break;
    }
  }

  pthread_join(tid, NULL);
  printf("main(read) thread exit.\n");
  return 0;
}
