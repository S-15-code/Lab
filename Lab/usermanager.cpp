#include "usermanager.h"

UserManager::UserManager(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_db(db)
{
}

bool UserManager::addUser(const User &user)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO User (name, role, department, contact, email) "
                  "VALUES (:name, :role, :dept, :contact, :email)");

    query.bindValue(":name", user.name);
    query.bindValue(":role", user.role);
    query.bindValue(":dept", user.department);
    query.bindValue(":contact", user.contact);
    query.bindValue(":email", user.email);

    if(!query.exec()) {
        qDebug() << "Error adding user:" << query.lastError().text();
        return false;
    }

    return true;
}

bool UserManager::updateUser(const User &user)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE User SET name=:name, role=:role, department=:dept, "
                  "contact=:contact, email=:email WHERE id=:id");

    query.bindValue(":id", user.id);
    query.bindValue(":name", user.name);
    query.bindValue(":role", user.role);
    query.bindValue(":dept", user.department);
    query.bindValue(":contact", user.contact);
    query.bindValue(":email", user.email);

    if(!query.exec()) {
        qDebug() << "Error updating user:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool UserManager::deleteUser(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM User WHERE id=:id");
    query.bindValue(":id", id);

    if(!query.exec()) {
        qDebug() << "Error deleting user:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<User> UserManager::getAllUsers()
{
    QList<User> userList;
    QSqlQuery query(m_db);

    if(!query.exec("SELECT * FROM User")) {
        qDebug() << "Error getting users:" << query.lastError().text();
        return userList;
    }

    while(query.next()) {
        User u;
        u.id = query.value("id").toInt();
        u.name = query.value("name").toString();
        u.role = query.value("role").toString();
        u.department = query.value("department").toString();
        u.contact = query.value("contact").toString();
        u.email = query.value("email").toString();

        userList.append(u);
    }

    return userList;
}

User UserManager::getUserById(int id)
{
    User u;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM User WHERE id=:id");
    query.bindValue(":id", id);

    if(!query.exec()) {
        qDebug() << "Error getting user by id:" << query.lastError().text();
        return u;
    }

    if(query.next()) {
        u.id = query.value("id").toInt();
        u.name = query.value("name").toString();
        u.role = query.value("role").toString();
        u.department = query.value("department").toString();
        u.contact = query.value("contact").toString();
        u.email = query.value("email").toString();
    }

    return u;
}

QList<User> UserManager::getUsersByRole(const QString &role)
{
    QList<User> userList;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM User WHERE role=:role");
    query.bindValue(":role", role);

    if(!query.exec()) {
        qDebug() << "Error getting users by role:" << query.lastError().text();
        return userList;
    }

    while(query.next()) {
        User u;
        u.id = query.value("id").toInt();
        u.name = query.value("name").toString();
        u.role = query.value("role").toString();
        u.department = query.value("department").toString();
        u.contact = query.value("contact").toString();
        u.email = query.value("email").toString();

        userList.append(u);
    }

    return userList;
}
