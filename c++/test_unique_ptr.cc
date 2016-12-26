#include <iostream>
#include <memory>

using namespace std;

unique_ptr<int>&& TestPassUniquePtr(unique_ptr<int>&& up) {
  return std::move(up);
}

unique_ptr<int> TestCreateUniquePtr(int i) {
  unique_ptr<int> temp_ptr(new int(i));
  return std::move(temp_ptr);
}

//Test content: the base usage of unique_ptr.
int main(int argc, char *argv[]) {
  unique_ptr<int> p1;
  unique_ptr<int> p2(nullptr);
  unique_ptr<int> p3(new int(10));
  unique_ptr<int> p4(new int(20));
  // the lvalue reference constructor has been deleted.
  unique_ptr<int> p5(std::move(p4));
  // the lvalue reference assignment operator= has been deleted.
  p1 = std::move(p3);
  std::cout << (p1 ? *p1 : 0) << std::endl;
  std::cout << (p2 ? *p2 : 0) << std::endl;
  std::cout << (p3 ? *p3 : 0) << std::endl;
  std::cout << (p4 ? *p4 : 0) << std::endl;
  std::cout << (p5 ? *p5 : 0) << std::endl;
  p2 = TestPassUniquePtr(std::move(p5));
  p3 = std::move(TestCreateUniquePtr(30));
  std::cout << "After Modified: " << std::endl;
  std::cout << (p1 ? *p1 : 0) << std::endl;
  std::cout << (p2 ? *p2 : 0) << std::endl;
  std::cout << (p3 ? *p3 : 0) << std::endl;
  std::cout << (p4 ? *p4 : 0) << std::endl;
  std::cout << (p5 ? *p5 : 0) << std::endl;
  return 0;
}
