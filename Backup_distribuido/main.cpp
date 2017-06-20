#include "backup_distribuido.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Backup_distribuido w;
    w.show();

    return a.exec();
}
