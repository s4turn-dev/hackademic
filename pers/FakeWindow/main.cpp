#include "mainwindow.h"
#include <QApplication>
#include "hook.h"
int main(int argc, char *argv[])
{
    installKeyboardHook();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
    removeKeyboardHook();
}
