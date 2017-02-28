#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>

//#include "lua.h"

typedef void* (ThreadRoutine)(void*);

typedef union Channel {
  int fds[2];
  struct {
    int rfd;
    int wfd;
  };
}Channel_t;

#define MAX_ERR_BUF_LEN 128
char err_buf[MAX_ERR_BUF_LEN];

Channel_t g_rrch; //request read channel
Channel_t g_rwch; //request write channel
Channel_t g_srch; //service read channel
Channel_t g_swch; //service write channel

void OpenChannel(Channel_t *);
void CreateThread(pthread_t *, ThreadRoutine *);

ThreadRoutine ClientRountine;
ThreadRoutine ServerRountine;
ThreadRoutine InnerServerRountine;
ThreadRoutine WorkRoutine;

int main(int argc, char *argv[]) {
  pthread_t client_td;
  pthread_t server_td;
  pthread_t inner_server_td;

  srand(time(NULL));

  signal(SIGPIPE, SIG_IGN);

  OpenChannel(&g_rrch);
  OpenChannel(&g_rwch);
  OpenChannel(&g_srch);
  OpenChannel(&g_swch);

  CreateThread(&client_td, &ClientRountine);
  CreateThread(&server_td, &ServerRountine);
  CreateThread(&inner_server_td, &InnerServerRountine);

  pthread_join(client_td, NULL);
  pthread_join(server_td, NULL);
  pthread_join(inner_server_td, NULL);

  return 0;
}

#define MAX_WRITE_TRY_TIMES 10
#define MAX_READ_TRY_TIMES 10
#define PIPE_ATOMIC_SIZE 4096

int ReadPipe(int fd, void* buf, size_t len) {
  int maxtry = MAX_READ_TRY_TIMES;
  size_t total = 0;
  size_t left = 0;
  size_t rlen = 0;
  ssize_t ret;
  while (total < len) {
    left = len - total;
    rlen = left;
    ret = read(fd, buf+total, rlen);
    if (ret == -1) {
      if ((errno == EAGAIN || errno == EWOULDBLOCK) && (maxtry > 0)) {
        maxtry--;
        sleep(1);
      } else {
        printf("Read pipe error: %s.\n", 
            strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
        return ret;
      }
    }
    if (ret == 0) {
      printf("Pipe write end has been close.\n");
      return -1;
    }
    if (ret > 0) {
      total += ret;
    }
  }
  return 0;
}

int WritePipe(int fd, void* buf, size_t len) {
  int maxtry = MAX_WRITE_TRY_TIMES;
  size_t total = 0;
  size_t left = 0;
  size_t wlen = 0;
  ssize_t ret;
  while (total < len) {
    left = len - total;
    wlen = left < PIPE_ATOMIC_SIZE ? left : PIPE_ATOMIC_SIZE;
    ret = write(fd, buf+total, wlen);
    if (ret == -1) {
      if ((errno == EAGAIN || errno == EWOULDBLOCK) && (maxtry > 0)) {
        maxtry--;
        sleep(1);
      } else {
        printf("Write pipe error: %s.\n", 
            strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
        return ret;
      }
    }
    total += wlen;
  }
  return 0;
}

void OpenChannel(Channel_t *pch) {
  int ret = pipe(pch->fds);
  if (ret != 0) {
    printf("Open pipe error: %s!\n", 
        strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
    exit(-1);
  }
  int flags = fcntl(pch->rfd, F_GETFL);
  ret = fcntl(pch->rfd, F_SETFL, flags | O_NONBLOCK);
  flags = fcntl(pch->wfd, F_GETFL);
  ret = fcntl(pch->wfd, F_SETFL, flags | O_NONBLOCK);
}

void CreateThread(pthread_t *ptid, ThreadRoutine *routine) {
  int ret = pthread_create(ptid, NULL, routine, NULL);
  if (ret != 0) {
    printf("Create Thread failed: %s!\n", 
        strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
    exit(-1);
  }
}

void* ClientRountine(void* arg) {
  printf("ClientRountine run.\n");
  int buf[10];
  int i;
  size_t wbs = 0;
  size_t total;
  int ret;
  int maxfd;
  fd_set rfs;
  for (i = 0; i < 10; ++i) {
    buf[i] = i;
  }
  total = sizeof(buf);
  ret = WritePipe(g_rwch.wfd, (void*)buf, total);
  if (ret != 0) {
    goto ClientRountineEnd;
  }
  maxfd = g_rrch.rfd + 1;
  for (i = 0; i < 10; ++i) {
    FD_SET(g_rrch.rfd, &rfs);
    ret = select(maxfd, &rfs, NULL, NULL, NULL);
    if (ret == -1) {
      printf("ClientRountineEnd select error: %s.\n", 
          strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
      goto ClientRountineEnd;
    }
    if (FD_ISSET(g_rrch.rfd, &rfs)) {
      ret = ReadPipe(g_rrch.rfd, (void*)(buf+i), sizeof(int));
      if (ret != 0) {
        goto ClientRountineEnd;
      }
    }
  }
  printf("Return result: \n");
  for (i = 0; i < 10; ++i) {
    printf("%d ", buf[i]);
  }
  printf("\n");

ClientRountineEnd:
  printf("ClientRountine end.\n");
  close(g_rwch.wfd);
  close(g_rrch.rfd);
  return NULL;
}

#define MAX_IDLE 10

void* ServerRountine(void* arg) {
  printf("ServerRountine run.\n");
  int req, resp;
  int ret, ret2;
  int maxfd;
  fd_set rfs;
  //struct timeval timeout;

  maxfd = g_rwch.rfd > g_srch.rfd ? g_rwch.rfd + 1 : g_srch.rfd + 1;

  while (1) {
    FD_ZERO(&rfs);
    FD_SET(g_rwch.rfd, &rfs);
    FD_SET(g_srch.rfd, &rfs);
    //timeout.tv_sec = 10;
    //timeout.tv_usec = 0;
    //ret = select(maxfd, &rfs, NULL, NULL, &timeout);
    ret = select(maxfd, &rfs, NULL, NULL, NULL);
    if (ret == -1) {
      printf("ServerRountine select error: %s.\n", 
          strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
      break;
    }
    if (ret == 0) {
      printf("ServerRountineEnd read timeout!\n");
      break;
    }
    if (FD_ISSET(g_rwch.rfd, &rfs)) {
      ret = ReadPipe(g_rwch.rfd, (void*)&req, sizeof(req));
      if (ret == -1) {
        printf("ServerRountine read requst channel error.\n");
        goto ServerRountineEnd;
      } else {
        ret2 = WritePipe(g_swch.wfd, (void*)&req, sizeof(req));
        if (ret2 == -1) {
          printf("ServerRountine write service channel error.\n");
          break;
        }
      }
    }
    if (FD_ISSET(g_srch.rfd, &rfs)) {
      ret = ReadPipe(g_srch.rfd, (void*)&resp, sizeof(resp));
      if (ret == -1) {
        printf("ServerRountine read service channel error.\n");
        goto ServerRountineEnd;
      } else {
        ret2 = WritePipe(g_rrch.wfd, (void*)&resp, sizeof(resp));
        if (ret2 == -1) {
          printf("ServerRountine write request channel error.\n");
          break;
        }
      }
    }
  }
ServerRountineEnd:
  printf("ServerRountine end.\n");
  close(g_rwch.rfd);
  close(g_rrch.wfd);
  close(g_swch.wfd);
  close(g_srch.rfd);
  return NULL;
}

void* InnerServerRountine(void* arg) {
  printf("InnerServerRountine run.\n");
  int req, resp;
  int ret, ret2;
  int maxfd;
  fd_set rfs;
  pthread_t tid;
  int buf[10];
  int pos = 0;

  maxfd = g_swch.rfd + 1;

  while (1) {
    FD_ZERO(&rfs);
    FD_SET(g_swch.rfd, &rfs);
    ret = select(maxfd, &rfs, NULL, NULL, NULL);
    if (ret == -1) {
      printf("InnerServerRountine select error: %s.\n", 
          strerror_r(errno, err_buf, MAX_ERR_BUF_LEN));
      break;
    }
    if (ret == 0) {
      printf("InnerServerRountine read timeout!\n");
      break;
    }
    if (FD_ISSET(g_swch.rfd, &rfs)) {
      ret = ReadPipe(g_swch.rfd, (void*)&req, sizeof(req));
      if (ret == -1) {
        printf("InnerServerRountine read service channel error.\n");
        goto InnerServerRountineEnd;
      } else {
        buf[pos] = req;
        ret2 = pthread_create(&tid, NULL, &WorkRoutine, (void*)(buf+pos));
        pos++;
        if (ret2 != 0) {
          printf("InnerServerRountine create work thread error: %s.\n", 
              strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
          ret2 = WritePipe(g_srch.wfd, (void*)&req, sizeof(req));
          if (ret2 != 0) {
            printf("InnerServerRountine write service channel error.\n");
            break;
          }
        } else {
          pthread_detach(tid);
        }
      }
    }
  }
InnerServerRountineEnd:
  printf("InnerServerRountine end.\n");
  close(g_swch.rfd);
  close(g_srch.wfd);
  return NULL;
}

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* WorkRoutine(void* arg) {
  long r = rand() % 10 + 1;
  int ret;
  printf("Request %d sleep %ld seconds.\n", *(int*)arg, r);
  sleep((unsigned int)r);
  //pthread_mutex_lock(&lock);
  ret = WritePipe(g_srch.wfd, arg, sizeof(int));
  //pthread_mutex_unlock(&lock);
  if (ret != 0) {
    printf("WorkRoutine write service channel error.\n");
  }
  return arg;
}
