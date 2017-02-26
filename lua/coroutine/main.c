#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

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

#define MAX_TRY_TIMES 10
#define PIPE_ATOMIC_SIZE 4096

int ReadPipe(int fd, void* buf, size_t len) {
  int maxtry = MAX_TRY_TIMES;

}

int WritePipe(int fd, void* buf, size_t len) {
  int maxtry = MAX_TRY_TIMES;
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
  ssize_t ret;
  size_t wbs = 0;
  size_t total;
  for (i = 0; i < 10; ++i) {
    buf[i] = i;
  }
  total = sizeof(buf);
  while (wbs < total) {
    ret = write(g_rwch.wfd, (void*)buf, (total-wbs));
    if (ret == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        sleep(1);
      } else {
        printf("ClientRountine write error: %s!\n", 
            strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        return NULL;
      }
    }
    wbs += ret;
  }
  for (i = 0; i < 10; ) {
    ret = read(g_rrch.rfd, (void*)(buf+i), sizeof(int));
    if (ret == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        sleep(1);
      } else {
        printf("ClientRountine read error: %s!\n", 
            strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        return NULL;
      }
    }
    if (ret == 0) {
      printf("Request Read Channel's write end has been closed \
          when %d times read operation.\n", i);
      return NULL;
    }
    if (ret == sizeof(int)) {
      ++i;
    }
  }
  printf("Return result: \n");
  for (i = 0; i < 10; ++i) {
    printf("%d ", buf[i]);
  }
  printf("\n");
  printf("ClientRountine end.\n");
  return NULL;
}

#define MAX_IDLE 10

void* ServerRountine(void* arg) {
  printf("ServerRountine run.\n");
  int req, resp;
  int max_idle = MAX_IDLE;
  int work_flag = 0;
  ssize_t ret, ret2;
  while(max_idle > 0) {
    work_flag = 0;
    ret = read(g_rwch.rfd, (void*)&req, sizeof(req));
    //printf("ServerRountine read ret(%d), req(%d).\n", ret, req);
    if (ret == -1) {
      if (errno != EAGAIN || errno != EWOULDBLOCK) {
        printf("ServerRountine read request channel error: %s!\n", 
            strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        return NULL;
      }
    }
    if (ret == 0) {
      printf("Request Write Channel's write end has been closed.\n");
      return NULL;
    }
    if (ret == sizeof(req)) {
      work_flag = 1;
      while (1) {
        ret2 = write(g_swch.wfd, (void*)&req, sizeof(req));
        if (ret2 == -1) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            sleep(1);
          } else {
            printf("ServerRountine write inner service channel error: %s!\n", 
                strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
            return NULL;
          }
        }
        if (ret2 == sizeof(req)) {
          break;
        }
      }
    }

    ret = read(g_srch.rfd, (void*)&resp, sizeof(resp));
    if (ret == -1) {
      if (errno != EAGAIN || errno != EWOULDBLOCK) {
        printf("ServerRountine read service channel error: %s!\n", 
            strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        return NULL;
      }
    }
    if (ret == 0) {
      printf("Service Read Channel's write end has been closed.\n");
      return NULL;
    }
    if (ret == sizeof(resp)) {
      work_flag = 1;
      while (1) {
        ret2 = write(g_rrch.wfd, (void*)&resp, sizeof(resp));
        if (ret2 == -1) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            sleep(1);
          } else {
            printf("ServerRountine write request channel error: %s!\n", 
                strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
            return NULL;
          }
        }
        if (ret2 == sizeof(resp)) {
          break;
        }
      }
    }

    if (work_flag == 0) {
      sleep(1);
      max_idle--;
    } else {
      max_idle = MAX_IDLE;
    }
  }
  printf("ServerRountine end.\n");
  return NULL;
}

void* InnerServerRountine(void* arg) {
  printf("InnerServerRountine run.\n");
  int req;
  int max_idle = MAX_IDLE;
  int work_flag = 0;
  ssize_t ret, ret2;
  pthread_t tid;
  int buf[10];
  int pos = 0;
  while(max_idle > 0) {
    work_flag = 0;
    ret = read(g_swch.rfd, (void*)&req, sizeof(req));
    //printf("InnerServerRountine read ret(%d), req(%d).\n", ret, req);
    if (ret == -1) {
      if (errno != EAGAIN || errno != EWOULDBLOCK) {
        printf("InnerServerRountine read service channel error: %s!\n", 
            strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        return NULL;
      }
    }
    if (ret == 0) {
      printf("Service Write Channel's write end has been closed.\n");
      return NULL;
    }
    if (ret == sizeof(req)) {
      work_flag = 1;
      buf[pos] = req;
      ret2 = pthread_create(&tid, NULL, &WorkRoutine, (void*)(buf+pos));
      pos++;
      if (ret2 != 0) {
        printf("InnerServerRountine create work thread error: %s.\n", 
            strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        while (1) {
          ret2 = write(g_srch.wfd, (void*)&req, sizeof(req));
          if (ret2 == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
              sleep(1);
            } else {
              printf("InnerServerRountine write inner service channel error: \
                  %s!\n", strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
              return NULL;
            }
          }
          if (ret2 == sizeof(req)) {
            break;
          }
        }
      } else {
        pthread_detach(tid);
      }
    }

    if (work_flag == 0) {
      sleep(1);
      max_idle--;
    } else {
      max_idle = MAX_IDLE;
    }
  }
  printf("InnerServerRountine end.\n");
  return NULL;
}

void* WorkRoutine(void* arg) {
  long r = rand() % 10 + 1;
  int ret;
  printf("Request %d sleep %ld seconds.\n", *(int*)arg, r);
  sleep((unsigned int)r);
  while (1) {
    ret = write(g_srch.wfd, arg, sizeof(int));
    if (ret == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        sleep(1);
      } else {
        printf("InnerServerRountine write inner service channel error: \
            %s!\n", strerror_r(ret, err_buf, MAX_ERR_BUF_LEN));
        return NULL;
      }
    }
    if (ret == sizeof(int)) {
      break;
    }
  }
  return arg;
}
