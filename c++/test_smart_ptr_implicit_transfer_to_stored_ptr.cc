#include <iostream>
#include <memory>

using namespace std;

class TestClass {
 public:
  TestClass(int x): x_(x) {
    std::cout << "TestClass constructed." << std::endl;
  }
  ~TestClass(){
    std::cout << "TestClass destructed." << std::endl;
  }
  void print() {
    std::cout << x_ << std::endl;
  }

 private:
  int x_;
};

int main(int argc, char *argv[]) {
  shared_ptr<TestClass> sptr(new TestClass(5));
  //There is no "operator T*()" like function in smart pointer class.
  //You Can't do it like below: 
  //TestClass *obj_ptr = (TestClass*)sptr;
  TestClass *obj_ptr = sptr.get();
  obj_ptr->print();
  return 0;
}
