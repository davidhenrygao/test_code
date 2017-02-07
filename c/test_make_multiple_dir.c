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
  size_t len = strlen(argv[1]);
  size_t pos;
  char mdirs[len+1];
  char temp[len+1];
  strncpy(mdirs, argv[1], len+1);
  if (mdirs[len-1] == '/') {
    mdirs[len-1] = '\0';
    len = len - 1;
  }
  pos = len;
  strncpy(temp, mdirs, len+1);
  //mode_t mode = umask(0);
  //printf("The file creation mask is : %04o.\n", mode);
  //umask(mode);
  while (pos) {
    if (access(temp, F_OK) == 0) {
      break;
    }
    while (pos > 0 && temp[--pos] != '/');
    temp[pos] = '\0';
  }
  while (pos < len) {
    while (pos < len) {
      pos++;
      if (mdirs[pos] == '/') {
        break;
      }
    }
    strncpy(temp, mdirs, pos);
    temp[pos] = '\0';
    if (mkdir(temp, S_IRWXU|S_IRWXG|S_IRWXO) != 0) {
      printf("Create directory error: %s!\n", strerror(errno));
      return -1;
    }
  }
  return 0;
}
