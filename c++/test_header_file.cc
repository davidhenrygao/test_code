#include <iostream>
#include <string>
#include <cstdio>

int main(int argc, char *argv[]) {
  std::string s = "Hello world!";
  std::cout << s << std::endl;
  printf("%s\n", s.c_str());
  return 0;
}
