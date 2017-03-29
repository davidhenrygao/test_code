#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int count = 0;
  printf("Process(%d) start.\n", getpid());
  while (1) {
    count++;
    printf("%d.\n", count);
    sleep(1);
  }
  return 0;
}
