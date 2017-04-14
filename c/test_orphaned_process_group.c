#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void sig_hup_handler(int sig) {
  printf("child catch signal %d.\n", sig);
}

void sig_hup_handler2(int sig) {
  printf("child2 catch signal %d.\n", sig);
  printf("child2 raise SIGKILL to itself.\n");
  if (raise(SIGKILL) != 0) {
    printf("child raise SIGKILL failed.\n");
  }
}

int set_signal_handler(int sig, void(*handler)(int)) {
  struct sigaction act;
  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  return sigaction(sig, &act, NULL);
}

int main(int argc, char *argv[]) {
  pid_t pid;
  int count = 0;
  printf("Program start.\n");
  printf("parent set signal(SIGHUP) handler.\n");
  //all its child will effect.
  if (set_signal_handler(SIGHUP, sig_hup_handler) < 0) {
    printf("parent set signal(SIGHUP) handler failed.\n");
    exit(-1);
  }
  pid = fork();
  if (pid > 0) {
    //parent process
    printf("parent's pid is %d.\n", getpid());
    sleep(5);
    printf("parent send SIGSTOP to child.\n");
    kill(pid, SIGSTOP);
  } else {
    printf("child's pid is %d.\n", getpid());
    while (1) {
      printf("child working: %d.\n", ++count);
      sleep(1);
      if (count % 10 == 0) {
        printf("child raise SIGSTOP to itself.\n");
        if (raise(SIGSTOP) != 0) {
          printf("child raise SIGSTOP failed.\n");
        }
      }
    }
    exit(0);
  }

  //fork the second child.
  printf("parent fork the second child.\n");
  pid = fork();
  if (pid == 0) {
    printf("child2(%d) start.\n", getpid());
    printf("child2 reset SIGHUP handler.\n");
    if (set_signal_handler(SIGHUP, sig_hup_handler2) < 0) {
      printf("child2 reset SIGHUP handler failed.\n");
      exit(-1);
    }
    while (1) {
      printf("child2 sleep.\n");
      sleep(1);
      printf("child2 wakeup.\n");
    }
    exit(0);
  }
  sleep(5);
  printf("parent exit.\n");
  return 0;
}
