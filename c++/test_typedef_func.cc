#include <iostream>

using namespace std;

typedef int (TestFunc) (int*);

class Test {
 public:
  Test(int x = 10) : i_(x) {}
  ~Test() {}
  virtual TestFunc MemFunc=0;
  int i_;

 private:
};

class Test2: public Test {
 public:
  Test2(int x = 20) : Test(x) {}
  ~Test2() {}
  TestFunc MemFunc;

 private:
};

//int Test2::MemFunc(int* ptr) {
TestFunc Test2::MemFunc(ptr) {
  *ptr = i_;
  return *ptr;
}

int main(int argc, char *argv[]) {
  Test2 obj;
  Test *pobj = (Test*)&obj;
  int x = 1;
  std::cout << "x = " << x << std::endl;
  std::cout << "MemFunc(&x) = " << pobj->MemFunc(&x) << std::endl;
  return 0;
}
