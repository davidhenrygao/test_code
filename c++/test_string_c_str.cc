#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {
  string s = "Hello world!";
  std::cout << s << std::endl;
  std::cout << "s length is: " << s.length() << std::endl;
  std::cout << "s max size is: " << s.max_size() << std::endl;
  std::cout << "s capacity is: " << s.capacity() << std::endl;
  string s2 = s;
  std::cout << s2 << std::endl;
  std::cout << "s2 length is: " << s2.length() << std::endl;
  std::cout << "s2 max size is: " << s2.max_size() << std::endl;
  std::cout << "s2 capacity is: " << s2.capacity() << std::endl;
  s2 = s.substr(0, 10);
  std::cout << "After copy 10 characters from s: " << std::endl;
  std::cout << s2 << std::endl;
  std::cout << "s2 length is: " << s2.length() << std::endl;
  std::cout << "s2 max size is: " << s2.max_size() << std::endl;
  std::cout << "s2 capacity is: " << s2.capacity() << std::endl;
  s2 = s.substr(0, 8);
  std::cout << "After copy 8 characters from s: " << std::endl;
  std::cout << s2 << std::endl;
  std::cout << "s2 length is: " << s2.length() << std::endl;
  std::cout << "s2 max size is: " << s2.max_size() << std::endl;
  std::cout << "s2 capacity is: " << s2.capacity() << std::endl;
  char buf[s.length()];
  s2 = strncpy(buf, s.c_str(), 6);
  std::cout << "After copy 6 characters from s: " << std::endl;
  std::cout << s2 << std::endl;
  std::cout << "s2 length is: " << s2.length() << std::endl;
  std::cout << "s2 max size is: " << s2.max_size() << std::endl;
  std::cout << "s2 capacity is: " << s2.capacity() << std::endl;
  s[5] = '\0';
  std::cout << "After set s[5] to \'\\0\', s is: " 
    << s << std::endl;
  std::cout << "s length is: " << s.length() << std::endl;
  std::cout << "s max size is: " << s.max_size() << std::endl;
  std::cout << "s capacity is: " << s.capacity() << std::endl;
  s2 = s.c_str();
  std::cout << "s2 length is: " << s2.length() << std::endl;
  std::cout << "s2 max size is: " << s2.max_size() << std::endl;
  std::cout << "s2 capacity is: " << s2.capacity() << std::endl;
  string emptystr;
  std::cout << emptystr << std::endl;
  std::cout << "emptystr length is: " << emptystr.length() << std::endl;
  std::cout << "emptystr max size is: " << emptystr.max_size() << std::endl;
  std::cout << "emptystr capacity is: " << emptystr.capacity() << std::endl;
  std::cout << "emptystr c_str is NULL: "<< boolalpha << 
    bool(emptystr.c_str()==NULL) << std::endl;

  return 0;
}
