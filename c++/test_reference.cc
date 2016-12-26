#include <iostream>
#include <memory>

using namespace std;

//Test content: the truth of reference in c++.
int main(int argc, char *argv[]) {
  int i = 1;
  int *ptr = &i;
  int j = 2;
  int& k = j;
  int&& rv_ref = (i+j);
  int&& rv_ref2 = 1;
  int x = 3;
  std::cout << "i address: " << &i << std::endl;
  std::cout << "unknowed interval address's(" << ptr-1 << ") value: " << *(ptr-1) << std::endl;
  std::cout << "ptr address: " << &ptr << std::endl;
  std::cout << "j address: " << &j << std::endl;
  std::cout << "k address: " << &k << std::endl;
  std::cout << "rv_ref address: " << &rv_ref << std::endl;
  std::cout << "rv_ref2 address: " << &rv_ref2 << std::endl;
  std::cout << "x address: " << &x << std::endl;

  std::cout << "Before rv_ref2: " << std::endl;
  long *ptr2 = (long*)(&rv_ref2 + 1);
  std::cout << "the first pointer address: "<< ptr2 << std::endl;
  std::cout << "the first pointer content: "<< hex << *ptr2 << std::endl;
  ptr2 += 1;
  std::cout << "the second pointer address: "<< ptr2 << std::endl;
  std::cout << "the second pointer content: "<< hex << *ptr2 << std::endl;
  ptr2 += 1;
  std::cout << "the third pointer address: "<< ptr2 << std::endl;
  std::cout << "the third pointer content: "<< hex << *ptr2 << std::endl;
  ptr2 += 1;
  std::cout << "the forth pointer address: "<< ptr2 << std::endl;
  std::cout << "the forth pointer content: "<< hex << *ptr2 << std::endl;
  ptr2 += 1;
  std::cout << "the fifth pointer address: "<< ptr2 << std::endl;
  std::cout << "the fifth pointer content: "<< hex << *ptr2 << std::endl;
  return 0;
}
