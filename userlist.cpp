#include "userlist.h"

UserList::UserList(QObject *parent) :
    QObject(parent)
{
}

bool UserList::addUser(QString name, int id)
{
    int i;
    for(i = 0; i < _userList.size(); ++i){
        QJsonObject obj = _userList.takeAt(i);
        if(obj["id"] == id) return false;
    }

    QJsonObject user;
    user["name"] = name;
    user["id"] = id;

    _userList.append(user);
}
