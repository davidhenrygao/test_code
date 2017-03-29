#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void sig_hup_handler(int sig) {
  printf("child catch signal %d.\n", sig);
  printf("child raise SIGSTOP to itself.\n");
  if (raise(SIGSTOP) != 0) {
    printf("child raise SIGSTOP failed.\n");
  }
}

int main(int argc, char *argv[]) {
  pid_t pid;
  struct sigaction act;
  int count = 0;
  printf("Program start.\n");
  pid = fork();
  if (pid > 0) {
    //parent process
    printf("parent's pid is %d.\n", getpid());
    sleep(5);
    printf("parent send SIGSTOP to child.\n");
    kill(pid, SIGSTOP);
    printf("parent exit.\n");
  } else {
    printf("child's pid is %d.\n", getpid());
    printf("child set signal(SIGHUP) handler.\n");
    act.sa_handler = sig_hup_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (sigaction(SIGHUP, &act, NULL) < 0) {
      printf("child set signal(SIGHUP) handler failed.\n");
      exit(-1);
    }
    while (1) {
      printf("child working: %d.\n", ++count);
      sleep(1);
    }
  }
  return 0;
}
