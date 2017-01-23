#include <iostream>

using namespace std;

class TestClass {
 public:
  TestClass(int x): x_(x) {}
  ~TestClass() {}
  void print1()const {
    std::cout << "print1 calling: x is " << x_ << std::endl;
  }
  void print2(int y)const {
    std::cout << "print2 calling: x is " << x_ 
      << "; y is " << y << "."<< std::endl;
  }
  static void print3() {
    std::cout << "print3 calling(static member function)." << std::endl;
  }

  int expose_x_;
  static int static_expose_x_;

 private:
  int x_;
};

int TestClass::static_expose_x_ = 10;

//NOTE: const must be written at last.
typedef void (TestClass::*PFUN1)()const;
typedef void (TestClass::*PFUN2)(int)const;
typedef void (*PFUN3)();
//class Member data.
typedef int (TestClass::*PMEM);
typedef int *PSMEM;

//NOTE: The theory of mem_fun and mem_fun_t, 
//      please check the c++ standard reference.
//      For c++11 use mem_fn and bind for instead.
int main(int argc, char *argv[]) {
  TestClass obj(1);
  TestClass* pObj = &obj;
  obj.print1();
  pObj->print2(2);
  pObj->print3();
  //PFUN1 pfun1 = &(TestClass::print2);  will compile error.
  PFUN1 pfun1 = &TestClass::print1;
  PFUN2 pfun2 = &TestClass::print2;
  PFUN3 pfun3 = &TestClass::print3;
  //obj.*pfun1(); or pObj->*pfun1(); will compile error.
  (obj.*pfun1)();
  (pObj->*pfun2)(3);
  //pfun3(); will work too.
  (*pfun3)();

  PMEM pdata = &TestClass::expose_x_;
  obj.*pdata = 1;
  obj.print2(obj.*pdata);
  PSMEM psdata = &TestClass::static_expose_x_;
  *psdata = 4;
  pObj->print2(*psdata);
  return 0;
}
