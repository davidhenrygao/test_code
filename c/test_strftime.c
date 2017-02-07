#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
  time_t t = time(NULL);
  struct tm *timeinfo = localtime(&t);
  char buf[32] = {'\0'};
  size_t ret = strftime(buf, 32, "%Y-%m-%d %q", timeinfo);
  printf("Current Date is: %s.\n", buf);
  printf("strftime return value is %u.\n", ret);
  return 0;
}
