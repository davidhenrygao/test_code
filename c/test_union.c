#include <stdio.h>
#include <unistd.h>
#include <errno.h>

union Channel {
  int fildes[2];
  struct {
    int rfd;
    int wfd;
  };
};

typedef union Channel Channel_t;

int main(int argc, char *argv[]) {
  Channel_t ch;
  int ret = pipe(ch.fildes);
  if (ret != 0) {
    perror("Create pipe failed");
  }
  printf("The channel read fd is %d.\n", ch.rfd);
  printf("The channel write fd is %d.\n", ch.wfd);
  return 0;
}
