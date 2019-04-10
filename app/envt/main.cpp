/*
 *  environment test
 *
 */

#include <QApplication>
#include <QtCore>
#include "window.h"
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc < 2) {
		std::cerr<<" not sufficient input, run: \nevnt filename \n to load exr image";
		return 1;
	}
	
    QApplication app(argc, argv);
    Window window(argv[1]);
    window.resize(512, 400);
    window.show();
    return app.exec();
}