#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define MAX_WRITE_TRY_TIMES 10
#define MAX_READ_TRY_TIMES 10
#define PIPE_ATOMIC_SIZE 4096

#define MAX_ERR_BUF_LEN 128
char err_buf[MAX_ERR_BUF_LEN];

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
