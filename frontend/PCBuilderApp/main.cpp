#include <QApplication>
#include "initdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InitDialog i;
    i.show();
    return a.exec();
}
