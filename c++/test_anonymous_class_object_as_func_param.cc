#include <iostream>

using namespace std;

class TestClass {
 public:
  TestClass(int x, int y): x_(x), y_(y) {}
  ~TestClass() {}
  void print()const {
    std::cout << x_ << ", " << y_ << std::endl;
  }

 private:
  int x_;
  int y_;
};

void TestFunc(const TestClass& obj) {
  obj.print();
}

int main(int argc, char *argv[]) {
  TestFunc(TestClass(1, 2));
  return 0;
}
