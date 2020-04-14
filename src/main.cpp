#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("SMDSUniAugsburg");
    QCoreApplication::setApplicationName("AutonomRemotecontrol");
    QCoreApplication::setOrganizationDomain("smds.informatik.uni-augsburg.de");
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
