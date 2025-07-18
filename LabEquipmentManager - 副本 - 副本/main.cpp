#include <QApplication>
#include "mainwindow.h"
#include "database.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 初始化数据库
    if (!Database::initialize()) {
        return 1;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
