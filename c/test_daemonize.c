#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/resource.h>


void daemonize() {
  pid_t pid;
  struct sigaction act;
  struct rlimit rlim;
  int nofile;
  int i;
  int fd0, fd1, fd2;

  printf("daemonize.\n");

  //clear the file mode creation mask
  umask(0);

  pid = fork();
  if (pid < 0) {
    printf("daemonize fork error: %s.\n", strerror(errno));
    exit(-1);
  }
  if (pid > 0) {
    exit(0);
  }
  if (setsid() < 0) {
    printf("daemonize setsid error: %s\n", strerror(errno));
    exit(-1);
  }

  //fork again, so the child process is not the session leader, 
  //it can't get a terminal.
  pid = fork();
  if (pid < 0) {
    printf("daemonize fork again error: %s.\n", strerror(errno));
    exit(-1);
  }
  if (pid > 0) {
    exit(0);
  }
  
  //change the working current directory to /,
  //so we won't prevent file system being unmounted.
  //here we just set ./
  if (chdir("./") < 0) {
    printf("daemonize chdir error: %s\n", strerror(errno));
    exit(-1);
  }

  //set the SIGHUP signal handler.
  act.sa_handler = SIG_IGN;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction(SIGHUP, &act, NULL) < 0) {
    printf("daemonize set SIGHUP handler error: %s.\n", strerror(errno));
    exit(-1);
  }

  //close all the open file.
  if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
    printf("daemonize get resource limit error: %s.\n", strerror(errno));
    exit(-1);
  }
  if (rlim.rlim_max == RLIM_INFINITY) {
    nofile = 1024;
  } else {
    nofile = rlim.rlim_max;
  }
  for (i = 0; i < nofile; ++i) {
    close(i);
  }

  //dup fd 0, 1, 2 to /dev/null
  fd0 = open("/dev/null", O_RDWR);
  if (fd0 < 0) {
    printf("daemonize open /dev/null error: %s\n", strerror(errno));
    exit(-1);
  }
  fd1 = dup(fd0);
  fd2 = dup(fd0);
  if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
    exit(-1);
  }
}

int main(int argc, char *argv[]) {
  FILE *fp;
  size_t wn;
  int count = 0;
  char buf[128];

  daemonize();
  printf("after daemonize.\n");
  fp = fopen("daemonize.log", "w+");
  if (fp == NULL) {
    exit(-1);
  }
  while (1) {
    snprintf(buf, 128, "%d\n", count);
    wn = fwrite(buf, strlen(buf), 1, fp);
    if (wn != 1) {
      exit(-1);
    }
    count++;
    fflush(fp);
    sleep(1);
  }
  return 0;
}
