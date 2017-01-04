#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main(int argc, char *argv[]) {

  typedef unordered_map<string, int> HMType;
  HMType hmap;
  hmap["hello"] = 1;
  hmap["world"] = 2;
  hmap["!"] = 3;

  for (HMType::const_iterator iter = hmap.begin(); iter != hmap.end(); ++iter) {
    std::cout << iter->first << iter->second << std::endl;
  }
  
  return 0;
}
