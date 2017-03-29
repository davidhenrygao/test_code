#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  mode_t org_mask = umask(0);
  printf("origin mask is %.8o.\n", org_mask);
  return 0;
}
