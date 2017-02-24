#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>

using namespace std;

int main(int argc, char *argv[]) {
  char buf[128];
  //It works! Because g++ compiler's strerror_r not use the buf you provide!
  string s = strerror_r(EAGAIN, NULL, 128);
  //string s = strerror_r(EAGAIN, buf, 128);
  std::cout << buf << std::endl;
  std::cout << s << std::endl;
  return 0;
}
