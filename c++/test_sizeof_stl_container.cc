#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

int main(int argc, char *argv[]) {
  string str;
  std::cout << "size of string is " << sizeof(str) << std::endl;
  vector<string> vec;
  std::cout << "size of vector is " << sizeof(vec) << std::endl;
  deque<string> deq;
  std::cout << "size of deque is " << sizeof(deq) << std::endl;
  list<string> lst;
  std::cout << "size of list is " << sizeof(lst) << std::endl;
  set<string> s;
  std::cout << "size of set is " << sizeof(s) << std::endl;
  map<string, int> m;
  std::cout << "size of map is " << sizeof(m) << std::endl;
  unordered_set<string> us;
  std::cout << "size of unordered_set is " << sizeof(us) << std::endl;
  unordered_map<string, int> um;
  std::cout << "size of unordered_map is " << sizeof(um) << std::endl;
  return 0;
}
