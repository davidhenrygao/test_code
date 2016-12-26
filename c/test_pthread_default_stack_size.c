#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define _GNU_SOURCE
#include <pthread.h>

void print_pthread_stack_size(char name[]) {
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
  /*
  err = pthread_attr_getstacksize(&attr, &stacksize);
  if (err != 0) {
    printf("Get Thread stacksize error:%s\n", strerror(errno));
    return;
  }
  printf("Thread(%d)'s stacksize is: %dKb\n", pid, stacksize/1024);
  */
  err = pthread_attr_getstack(&attr, &stackaddr, &stacksize);
  if (err != 0) {
    printf("Get Thread stackaddr error:%s\n", strerror(errno));
    return;
  }
  printf("Thread(%s:%d)'s stackaddr is: %p\n", name, pid, stackaddr);
  printf("Thread(%s:%d)'s stacksize is: %dKb\n", name, pid, stacksize/1024);
  sleep(100);
}
void* thread_main(void *arg) {
  print_pthread_stack_size("child");
  return NULL;
}

int main(int argc, char *argv[]) {
  int err;
  int a = 1;
  pthread_t pid;
  int* ptr = (int*)malloc(128*1024);
  printf("pointer length is: %u.\n", sizeof(ptr));
  printf("Stack allocate memory address: %p.\n", &a);
  printf("Heap allocate memory address: %p.\n", ptr);
  printf("Create a thread now.\n");
  err = pthread_create(&pid, NULL, &thread_main, NULL);
  if (err != 0) {
    printf("Create thread error: %s\n", strerror(errno));
    return -1;
  }
  print_pthread_stack_size("main");
  err = pthread_join(pid, NULL);
  printf("Thread is terminated.\n");
  free(ptr);
  ptr = NULL;
  return 0;
}
