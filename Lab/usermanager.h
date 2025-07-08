#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QDebug>

struct User {
    int id;
    QString name;
    QString role;  // admin, teacher, student
    QString department;
    QString contact;
    QString email;
};

class UserManager : public QObject
{
    Q_OBJECT
public:
    explicit UserManager(QSqlDatabase db, QObject *parent = nullptr);

    bool addUser(const User &user);
    bool updateUser(const User &user);
    bool deleteUser(int id);
    QList<User> getAllUsers();
    User getUserById(int id);
    QList<User> getUsersByRole(const QString &role);

private:
    QSqlDatabase m_db;
};

#endif // USERMANAGER_H
