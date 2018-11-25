#include <iostream>
#include <string>
#include <ctime>
#include <sdb/Array.h>
#include <sdb/Types.h>

using namespace alo;

struct Visibility {
  bool _isVisible;
};

bool testArray()
{
  sdb::Array<sdb::Coord2, Visibility> arr;
  const int g[5] = {2, 1, 3, 0, 4};
  for(int i=0;i<5;++i) {
    for(int j=0;j<10000;++j) {
      int k = 999 + (rand() % 131071);
      arr.insert(sdb::Coord2(k, i), new Visibility() );
    }
  }

  std::cout << "\n array size " << arr.size();
  return true;
}

int main(int argc, char *argv[])
{
  std::time_t timer;
  std::time(&timer);
  std::cout << "\n " << std::asctime(std::localtime(&timer));

  if(!testArray()) return 1;

  std::time(&timer);
  std::cout << "\n " << std::asctime(std::localtime(&timer));
  return 0;
}
