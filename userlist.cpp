#include "userlist.h"

#include <QFile>
#include <QJsonDocument>
#include <QStringList>

#include <QDebug>

UserList::UserList(QObject *parent) : QObject(parent)
{
    load();
}

bool UserList::addUser(QString name, int id)
{
    int i;
    for(i = 0; i < _userList.size(); ++i){
        QJsonObject obj = _userList.takeAt(i).toObject();
        if(obj["id"] == id) return false;
    }

    QJsonObject user;
    user["name"] = name;
    user["id"] = id;

    _userList.append(user);

    return true;
}

void UserList::save()
{
    QJsonObject userList;
    userList["userlist"] = _userList;

    QJsonDocument doc(userList);

    QFile file(USER_SAVE);
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();
}

void UserList::load()
{
    QFile file(USER_SAVE);
    if(file.exists()){

        file.open(QIODevice::ReadOnly | QIODevice::Text);

        QString content = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());

        QJsonObject obj = doc.object();

        _userList = obj["userlist"].toArray();
    }
}

QStringList UserList::toList()
{
    QStringList list;

    int i;
    for(i = 0; i < _userList.size(); ++i){
        QJsonObject obj = _userList.takeAt(i).toObject();
        QString content;

        content.append(obj["name"].toString() + " : " + QString("%1").arg(obj["id"].toInt()));
        qDebug() << content;
        list << content;
    }

    return list;
}


