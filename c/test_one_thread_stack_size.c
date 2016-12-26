#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define _GNU_SOURCE
#include <pthread.h>

/*
void print_pthread_stack_size() {
  int err;
  size_t stacksize;
  void *stackaddr;
  pthread_attr_t attr;
  pthread_t pid = pthread_self();

  err = pthread_getattr_np(pid, &attr);
  if (err != 0) {
    printf("Get Thread attributes error:%s\n", strerror(errno));
    return;
  }
  err = pthread_attr_getstack(&attr, &stackaddr, &stacksize);
  if (err != 0) {
    printf("Get Thread stackaddr error:%s\n", strerror(errno));
    return;
  }
  printf("Thread(%d)'s stackaddr is: %p\n", pid, stackaddr);
  printf("Thread(%d)'s stacksize is: %dKb\n", pid, stacksize/1024);
}
*/

int main(int argc, char *argv[]) {
  int i;
  char a[1*1024*1024];
  for (i = 0; i < sizeof(a); i+=4*1024) {
    a[i] = 1;
  }
  //print_pthread_stack_size();
  pause();
  return 0;
}
