#include "logindialog.h"
#include <QApplication>
#include <QStyle>
#include <QScreen>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    loginSuccess(false)
{
    setupUI();

    // 设置窗口属性
    setWindowTitle("实验室设备管理系统 - 管理员登录");
    setFixedSize(350, 200);
    setModal(true);

    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;//（屏幕总宽度-窗口宽度）/2=从左侧开始应留出的宽度，居中位置
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel *titleLabel = new QLabel("管理员登录");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);//把设置的东西加入窗口

    // 用户名输入
    QHBoxLayout *usernameLayout = new QHBoxLayout;
    QLabel *usernameLabel = new QLabel("账号:");
    usernameLabel->setFixedWidth(60);
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("请输入管理员账号");
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameEdit);
    mainLayout->addLayout(usernameLayout);

    // 密码输入
    QHBoxLayout *passwordLayout = new QHBoxLayout;
    QLabel *passwordLabel = new QLabel("密码:");
    passwordLabel->setFixedWidth(60);
    passwordEdit = new QLineEdit;
    passwordEdit->setEchoMode(QLineEdit::Password);
    //setechomode 设置密码模式：normal noecho password掩 PasswordEchoOnEdit明-》掩
    passwordEdit->setPlaceholderText("请输入管理员密码");
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    mainLayout->addLayout(passwordLayout);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    loginButton = new QPushButton("登录");
    loginButton->setDefault(true);//设置为默认按钮：enter自动触发该按钮
    cancelButton = new QPushButton("取消");

    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 连接信号
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::onCancelClicked);
    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    // 设置焦点：当前接收键盘输入的控件
    usernameEdit->setFocus();
}

void LoginDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {// 检查是否按下了ESC键
        onCancelClicked();
    } else {
        QDialog::keyPressEvent(event);// 其他按键交给父类默认按键事件处理机制处理：其他按键的功能不变，父类已经有处理机制
    }
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();//去除字符串首尾的空白
    QString password = passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入账号密码！");
        return;
    }

    if (validateUser(username, password)) {
        loginSuccess = true;
        QMessageBox::information(this, "成功", "登录成功");
        accept();//关闭对话框并返回QDialog::Accepted==>main中判断后，才能打开窗口
    } else {
        QMessageBox::critical(this, "错误", "账号或密码错误！");//专用于显示 严重错误 级别的消息
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::onCancelClicked()
{
    loginSuccess = false;
    reject();
}

bool LoginDialog::validateUser(const QString &username, const QString &password)
{
    // 固定的管理员账号密码
    const QString ADMIN_USERNAME = "诸葛亮";
    const QString ADMIN_PASSWORD = "666666";

    return (username == ADMIN_USERNAME && password == ADMIN_PASSWORD);
} 
