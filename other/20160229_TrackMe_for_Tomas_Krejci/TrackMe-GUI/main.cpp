#include "mainwindow.h"

#include "config.h"
#include <QApplication>
#include <QAbstractAnimation>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    config();
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
