#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  printf("execute program.\n");
  sleep(2);
  system("ps aux | grep pts");
  sleep(2);
  printf("exit program.\n");
  return 0;
}
