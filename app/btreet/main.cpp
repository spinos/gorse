#include <iostream>
#include <string>
#include <ctime>
#include <sdb/Types.h>
#include <sdb/L3Tree.h>

using namespace alo;

struct Visibility {
  bool _isVisible;
};

bool testArray()
{
  sdb::L3Tree<sdb::Coord2, Visibility *, 1024, 1024> arr;
  const int g[5] = {4, 0, 3, 5, 2};
  for(int i=0;i<5;++i) {
    for(int j=0;j<21755;++j) {
      int k = 999 + j;// + (rand() & 33554431);
      //std::cout<< "\n add " << k << ", "<< g[i];
      arr.insert(sdb::Coord2(k, g[i]), new Visibility() );

    }
  }

  arr.printDetail();

  std::cout << " find (1000, 0)" << *arr.find(sdb::Coord2(1000,0));

  std::vector<sdb::L3Node<sdb::Coord2, Visibility *, 1024> *>::const_iterator it = arr.begin();
  for(;it!=arr.end();++it) {}

  for(int i=0;i<5;++i) {
    int n = 20754;
    if(i>2) n = 21755;
    for(int j=0;j<n;++j) {
      int k = 999 + j;// + (rand() & 33554431);
      //std::cout<< "\n add " << k << ", "<< g[i];
      arr.remove(sdb::Coord2(k, g[i]) );

    }
  }

  arr.printDetail();

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
