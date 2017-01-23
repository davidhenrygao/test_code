#include <iostream>
#include <string>
#include <algorithm>
#include <utility>
#include <functional>
#include <cctype>

using namespace std;

bool ciCharLess(char c1, char c2) {
  return tolower(static_cast<unsigned char>(c1)) < 
         tolower(static_cast<unsigned char>(c2));
}

bool ciStringCompare(const string &s1, const string &s2) {
  return lexicographical_compare(s1.begin(), s1.end(), 
                                 s2.begin(), s2.end(),
                                 ciCharLess);
}

int ciCharCompare(char c1, char c2) {
  int ic1 = tolower(static_cast<unsigned char>(c1));
  int ic2 = tolower(static_cast<unsigned char>(c2));
  if (ic1 < ic2) {
    return -1;
  }
  if (ic1 > ic2) {
    return 1;
  }
  return 0;
}

int ciStringCompareImpl(const string &s1, const string &s2) {
  typedef pair<string::const_iterator, string::const_iterator> PSI;
  PSI res_pair = mismatch(s1.begin(), s1.end(), s2.begin(), 
                          not2(ptr_fun(ciCharCompare)));
  if (res_pair.first == s1.end()) {
    if (res_pair.second == s2.end()) {
      return 0;
    } else {
      return -1;
    }
  }
  return ciCharCompare(*(res_pair.first), *(res_pair.second));
}

int ciStringCompare2(const string &s1, const string &s2) {
  if (s1.size() <= s2.size()) {
    return ciStringCompareImpl(s1, s2);
  } else {
    return -ciStringCompareImpl(s2, s1);
  }
}

int main(int argc, char *argv[]) {
  string s1;
  string s2;
  std::cout << "Please Input the string 1: " << std::endl;
  std::cin >> s1;
  std::cout << "Please Input the string 2: " << std::endl;
  std::cin >> s2;
  std::cout << "Compare \"" << s1 << "\" and \"" << s2 << "\"" << std::endl;
  std::cout << "The result is (use lexicographical_compare): " 
    << ciStringCompare(s1, s2) << std::endl;
  std::cout << "The result is (use mismatch): " 
    << ciStringCompare2(s1, s2) << std::endl;
  return 0;
}
