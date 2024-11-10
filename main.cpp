#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    mainwindow w;
    w.resize(800,600);
    w.show();
//    std::ofstream file("data.txt", std::ios::app);
//    file.clear();
    return a.exec();
}
