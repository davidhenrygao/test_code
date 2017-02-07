#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  size_t wl;
  char buf[] = "Hello world!";
  size_t bl = sizeof(buf)/sizeof(char) - 1;
  FILE *fp = fopen("./log/test.log", "w+");
  if (fp == NULL) {
    perror("Open file error");
    return -1;
  }
  wl = fwrite(buf, sizeof(char), bl,  fp);
  if (wl < bl) {
    printf("Write error!\n");
    return -1;
  }
  fclose(fp);
  fp = NULL;
  return 0;
}
