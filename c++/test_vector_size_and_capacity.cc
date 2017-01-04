#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void print(int i) {
  std::cout << i << " ";
}

int main(int argc, char *argv[]) {
  vector<int> vec1;
  std::cout << "vec1 size is : "<< vec1.size() << std::endl;
  std::cout << "vec1 capacity is : " << vec1.capacity() << std::endl;
  vec1.reserve(10);
  std::cout << "After reserve 10: " << std::endl;
  std::cout << "vec1 size is : "<< vec1.size() << std::endl;
  std::cout << "vec1 capacity is : " << vec1.capacity() << std::endl;
  vec1.resize(5);
  std::cout << "After resize 5: " << std::endl;
  std::cout << "vec1 size is : "<< vec1.size() << std::endl;
  std::cout << "vec1 capacity is : " << vec1.capacity() << std::endl;
  int i;
  for (i = 0; i < vec1.size(); ++i) {
    vec1[i] = i;  //operator[] is just an access member function!
  }
  std::cout << "After init loop: " << std::endl;
  std::cout << "vec1 size is : "<< vec1.size() << std::endl;
  std::cout << "vec1 capacity is : " << vec1.capacity() << std::endl;
  vec1.resize(6);
  std::cout << "After resize 6: " << std::endl;
  std::cout << "print the vector: " << std::endl;
  for_each(vec1.begin(), vec1.end(), print);
  std::cout << std::endl;
  vec1.clear();
  std::cout << "After clear vec1: " << std::endl;
  std::cout << "vec1 size is : "<< vec1.size() << std::endl;
  std::cout << "vec1 capacity is : " << vec1.capacity() << std::endl;
  std::cout << std::endl;

  //Constructors
  vector<int> vec2;  // default
  vector<int> vec3(5, 10);  //fill
  vector<int> vec4(vec1.begin(), vec1.end()); //range
  vector<int> vec5(vec3); // copy
  vector<int> vec6(std::move(vec5));  // move
  vector<int> vec7 = {9,8,7,6,5,4,3,2,1}; //initialized_list
  std::cout << "print the vector2: " << std::endl;
  for_each(vec2.begin(), vec2.end(), print);
  std::cout << std::endl;
  std::cout << "print the vector3: " << std::endl;
  for_each(vec3.begin(), vec3.end(), print);
  std::cout << std::endl;
  std::cout << "print the vector4: " << std::endl;
  for_each(vec4.begin(), vec4.end(), print);
  std::cout << std::endl;
  std::cout << "print the vector5: " << std::endl;
  for_each(vec5.begin(), vec5.end(), print);
  std::cout << std::endl;
  std::cout << "print the vector6: " << std::endl;
  for_each(vec6.begin(), vec6.end(), print);
  std::cout << std::endl;
  std::cout << "print the vector7: " << std::endl;
  for_each(vec7.begin(), vec7.end(), print);
  std::cout << std::endl;

  return 0;
}
