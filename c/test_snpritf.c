#include <stdio.h>

int main(int argc, char *argv[]) {
  char buf[5];
  int ret = snprintf(buf, 5, "Hello World");
  printf("string: %s\n", buf);
  printf("return code: %d\n", ret);
  return 0;
}
