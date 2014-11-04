#ifndef USERLIST_H
#define USERLIST_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#define USER_SAVE "users.json"

class UserList : public QObject
{
    Q_OBJECT
public:
    explicit UserList(QObject *parent = 0);

    struct User{
        QString name;
        int id;
    };

    bool addUser(QString name, int id);
    void save();
    void load();

signals:

public slots:

private:
    QJsonArray _userList;

};

#endif // USERLIST_H
