#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#define MAX_EVENTS 16

int pipefd[2];

void SetNonBlock(int fd);
void handle_data_sock(int epollfd, struct epoll_event *pev);

void* worker_thread_routine(void*);

//not check any error in this example.
int main(int argc, char *argv[]) {
  int sockfd, connfd;
  int epollfd;
  struct sockaddr_in addr;
  struct sockaddr_in cliaddr;
  socklen_t addrlen;
  struct epoll_event ev;
  struct epoll_event evs[MAX_EVENTS];
  int ens;
  int i;
  pthread_t pid;

  //ingnore signal PIPE.
  signal(SIGPIPE, SIG_IGN);

  pipe(pipefd);

  pthread_create(&pid, NULL, &worker_thread_routine, (void*)&pipefd[0]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(2017);
  addr.sin_addr.s_addr = INADDR_ANY;
  //addr.sin_addr.s_addr = inet_addr("192.168.82.198");
  
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&reuse, sizeof(reuse));

  bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
  listen(sockfd, SOMAXCONN);

  epollfd = epoll_create(1);
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = sockfd;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

  printf("Server start.\n");

  for(;;) {
    printf("epoll_wait.\n");
    ens = epoll_wait(epollfd, evs, MAX_EVENTS, -1);
    for (i = 0; i < ens; ++i) {
      if (evs[i].data.fd == sockfd) {
        //memset(&cliaddr, 0, sizeof(cliaddr));
        addrlen = sizeof(cliaddr);
        connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &addrlen);
        printf("accept connection from %s:%u\n", 
            inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        SetNonBlock(connfd);
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = connfd;
        printf("epoll add connfd %d.\n", connfd);
        epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev);
      } else {
        handle_data_sock(epollfd, &evs[i]);
      }
    }
  }
  return 0;
}

void SetNonBlock(int fd) {
  int flags;
  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

#define BUFFSIZE 64
char rbuf[BUFFSIZE] = {"\0"};
char wbuf[BUFFSIZE] = {"\0"};

void handle_data_sock(int epollfd, struct epoll_event *pev) {
  int fd = pev->data.fd;
  ssize_t rb, wb;
  char c;
  struct epoll_event ev;
  if (pev->events & EPOLLERR) {
    printf("EPOLLERR event found.\n");
    //epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
  }
  if (pev->events & EPOLLHUP) {
    printf("EPOLLHUP event found.\n");
    //epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
  }
  if (pev->events & EPOLLIN) {
    printf("EPOLLIN event found.\n");
    rb = read(fd, (void*)rbuf, BUFFSIZE-1);
    if (rb < 0) {
      if (errno != EAGAIN || errno != EWOULDBLOCK) {
        printf("read socket %d error: %s.\n", fd, strerror(errno));
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
        close(fd);
      }
    }
    if (rb == 0) {
      printf("socket peer end close the connetion.\n");
      epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
      close(fd);
    }
    if (rb > 0) {
      rbuf[rb] = '\0';
      printf("recv client msg: %s.\n", rbuf);
      strncpy(wbuf, "echo back msg.", BUFFSIZE);
      ev.events = EPOLLOUT | EPOLLET;
      ev.data.fd = fd;
      epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
      char c = rbuf[0];
      if (c == 'c') {
        write(pipefd[1], &fd, sizeof(fd));
      }
    }
  }
  if (pev->events & EPOLLOUT) {
    printf("EPOLLOUT event found.\n");
    wb = write(fd, (void*)wbuf, strlen(wbuf));
    if (wb < 0) {
      if (errno != EAGAIN || errno != EWOULDBLOCK) {
        printf("write socket %d error: %s.\n", fd, strerror(errno));
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
        close(fd);
      }
    }
    if (wb > 0) {
      ev.events = EPOLLIN | EPOLLET;
      ev.data.fd = fd;
      epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    }
  }
}

void* worker_thread_routine(void* arg) {
  printf("worker thread start.\n");
  int rpfd = *(int*)arg;
  int fd;
  for(;;) {
    read(rpfd, &fd, sizeof(fd));
    printf("read fd(%d) from pipe.\n", fd);
    sleep(1);
    //close(fd) or shutdown(fd, SHUT_RD) will not generate EPOLLHUP event.
    printf("shutdown fd(%d).\n", fd);
    shutdown(fd, SHUT_WR);
    //shutdown(fd, SHUT_RD);
    //shutdown(fd, SHUT_RDWR);
    //printf("close fd(%d).\n", fd);
    //close(fd);
  }
}
