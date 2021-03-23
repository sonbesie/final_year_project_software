#include "controller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Controller* w = new Controller();
    w->show();
    return a.exec();
}
