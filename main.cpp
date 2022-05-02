#include "MainWindow.h"
#include <QApplication>
#include <QDebug>
#include <QWidget>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
