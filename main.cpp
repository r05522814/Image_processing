#include "mainwindow.h"
#include <QApplication>

using namespace cv;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    std::cout<<"test output"<<endl;

    return a.exec();
}
