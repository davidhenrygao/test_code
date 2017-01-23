#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

//Note: The result of the sequential lock and unlock operations is 
//      base on the type of pthread_mutexattr_t attr.
//      Please read man page for detail.
//      In my opinion, I think PTHREAD_MUTEX_NORMAL type is more reasonable.
int main(int argc, char *argv[]) {
  pthread_mutexattr_t attr;
  pthread_mutex_t mutex;
  int ret = pthread_mutexattr_init(&attr);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("init mutex attr success!\n");
  //ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT);
  //ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  //ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
  ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("mutex attr set type success!\n");
  ret = pthread_mutex_init(&mutex, &attr);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("init mutex success!\n");
  ret = pthread_mutexattr_destroy(&attr);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("destroy mutex attr success!\n");
  ret = pthread_mutex_unlock(&mutex);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("unlock mutex success!\n");
  ret = pthread_mutex_lock(&mutex);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("lock mutex success!\n");
  ret = pthread_mutex_lock(&mutex);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("lock mutex success!\n");
  /*ret = pthread_mutex_unlock(&mutex);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("unlock mutex success!\n");*/
  ret = pthread_mutex_destroy(&mutex);
  if (ret != 0) {
    printf("Error: %s\n",strerror(ret));
    return -1;
  }
  printf("destroy mutex success!\n");
  return 0;
}
