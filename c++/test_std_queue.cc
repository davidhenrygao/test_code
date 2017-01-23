#include <iostream>
#include <queue>
#include <list>

using namespace std;

int main(int argc, char *argv[]) {
  queue< int, list<int> > q;
  int elem = q.front();
  std::cout << elem << std::endl;
  return 0;
}
