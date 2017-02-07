#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: prog dirpath\n");
    return -1;
  }
  if (access(argv[1], F_OK) == 0) {
    printf("path(%s) is exist!\n", argv[1]);
  } else {
    printf("path(%s) is not exist!\n", argv[1]);
  }
  return 0;
}
