#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int fds[2];
  pid_t pid;
  char buf[60];
  unsigned int sec = 60;
  const char *msg = "Hello father";
  if (pipe(fds) < 0) {
    printf("Open pipe error!\n");
    exit(-1);
  }
  printf("parent(%d) start.\n", getpid());
  pid = fork();
  if (pid == 0) {
    //child
    printf("Child(%d) start.\n", getpid());
    close(fds[0]); //close read end.
    printf("Child sleep %u seconds.\n", sec);
    sleep(sec);
    printf("Child write string: %s.\n", msg);
    write(fds[1], msg, strlen(msg)+1);
  } else {
    //parent
    close(fds[1]); //close write end.
    printf("Parent wait for child's msg.\n");
    read(fds[0], buf, sizeof(buf));
    printf("Parent read child's msg(%s).\n", buf);
  }
  return 0;
}
