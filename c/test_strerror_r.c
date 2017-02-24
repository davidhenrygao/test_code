#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  char buf[128];
  strerror_r(EAGAIN, buf, 128);
  printf("%s\n", buf);
  return 0;
}
