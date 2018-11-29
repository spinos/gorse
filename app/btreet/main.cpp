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

  //arr.printDetail();

  sdb::L3DataIterator<sdb::Coord2, Visibility *, 1024>dit = arr.begin(sdb::Coord2(0,0));
  if(dit.done()) std::cout << "(*,0) not found";
  else std::cout << " begin in 0 " << dit.first;
  int n0 = 0;
  for(;!dit.done();dit.next()) {
      if(dit.first.y > 0) break;
      n0++;
  }
  std::cout << " end in 0 " << dit.first << " count " << n0;

  sdb::L3Node<sdb::Coord2, Visibility *, 1024> *d = arr.begin();
  while(d) {
    d = arr.next(d);
  }

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
