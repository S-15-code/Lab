#include <QApplication>
#include "mainwindow.h"
#include "database.h"
#include "logindialog.h"

int main(int argc, char *argv[]) {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    QApplication a(argc, argv);

    // 初始化数据库
    if (!Database::initialize()) {
        return 1;
    }
    // 显示登录对话框
        LoginDialog loginDialog;
        if (loginDialog.exec() != QDialog::Accepted) {
            return 0; // 用户取消登录，退出程序
        }

        // 登录成功，显示主窗口
        MainWindow w;
        w.show();

    return a.exec();
}

