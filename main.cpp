#include<QApplication>
#include"shell.h"

#include <QDebug>
//#define wid_test
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

#ifndef wid_test
    shell w;
    if(argc==2)
        w.open(argv[1]);
#else
    grid_net grid;
    Paint_view w(grid);
#endif
    w.show();
    return app.exec();
}
