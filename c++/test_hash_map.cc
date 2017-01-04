#include <iostream>
#include <string>
#include <ext/hash_map>

using namespace std;

int main(int argc, char *argv[]) {
  //Note: __gnu_cxx::hash_map<string, int> will compile error, because hash method not support string type.
  //      But you can write something like below to solve it.
  //namespace __gnu_cxx
  //{
  //    template<> struct hash< std::string >
  //    {
  //        size_t operator()( const std::string& x ) const
  //        {
  //            return hash< const char* >()( x.c_str() );
  //        }
  //    };
  //}

  typedef __gnu_cxx::hash_map<int, string> HMType;
  HMType hmap;
  hmap[1] = "hello";
  hmap[2] = "world";
  hmap[3] = "!";

  for (HMType::const_iterator iter = hmap.begin(); iter != hmap.end(); ++iter) {
    std::cout << iter->first << iter->second << std::endl;
  }
  
  return 0;
}
