#include <iostream>

using namespace std;

#include <pthread.h>

class TestThread {
 public:
  TestThread() {}
  ~TestThread() {}
  void Routine(void);
  bool Start();
  bool Join();

 private:
  pthread_t tid_;
  static void* RoutineAdaptor(void*);
};

void* TestThread::RoutineAdaptor(void *arg) {
  TestThread *pobj = (TestThread*)arg;
  pobj->Routine();
  return NULL;
}

void TestThread::Routine() {
  std::cout << "Test class Routine: thread_id: " << tid_ << std::endl;
  std::cout << "Created thread exit." << std::endl;
  return;
}

bool TestThread::Start() {
  int ret = pthread_create(&tid_, NULL, &RoutineAdaptor, this);
  if (ret != 0) {
    std::cout << "Thread start error: " << ret << std::endl;
    return false;
  }
  return true;
}

bool TestThread::Join() {
  int ret = pthread_join(tid_, NULL);
  if (ret != 0) {
    std::cout << "Join failed!" << std::endl;
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  std::cout << "Create a new thread." << std::endl;
  TestThread t;
  std::cout << "Start the thread." << std::endl;
  t.Start();
  std::cout << "Join the thread." << std::endl;
  t.Join();
  std::cout << "Main thread exit." << std::endl;
  return 0;
}
