/*
 *  rng test
 *  aloe
 */
#include <QApplication>
#include <QDesktopWidget>
#include <QtCore>
#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window window;
    //window.showMaximized();
    window.resize(480, 480);
    window.show();
    return app.exec();
}

/*
#include <iostream>
#include <ctime>
#include <vector>
#include <map>
#include <math/QuickSort.h>
#include <rng/Lehmer.h>
#include <rng/Uniform.h>

using namespace alo;

void checkRepeat(int* vis, int n)
{
	QuickSort1::Sort<int>(vis, 0, n-1);
	for(int i=1;i<n;++i) {
		if(vis[i] == vis[i-1]) {
			std::cout<<"\n ERROR report "<<i<<" "<<vis[i];
		}
	}
}

void testLcg()
{
	std::cout<<"\n test lehmer lcg ";
	std::time_t secs = std::time(0);
	std::cout << secs << " seconds since the Epoch\n";
	srand(secs);
			  
	int vis[1000];
	
	Uniform<Lehmer> lmlcg(secs);
	for(int i=0;i<1000;++i) {
		int j = lmlcg.rand(1<<22);
		//int j = rand() % 1001;
		vis[i] = j;
		int k = j >> 15;
	}
	
	checkRepeat(vis, 1000);
}
*/
