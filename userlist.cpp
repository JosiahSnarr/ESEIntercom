#include "userlist.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

#include <QDebug>

UserList::UserList(QObject *parent) : QObject(parent)
{
    load();
}

bool UserList::addUser(QString name, int id)
{
    std::vector<User>::iterator iter;

    // check if the id exists already
    for(iter = _userList.begin(); iter != _userList.end(); ++iter){
        if((*iter).id == id) return false;
    }

    User user;
    user.id = id;
    user.name = name;

    _userList.push_back(user);

    return true;
}

void UserList::save()
{
    QFile file(USER_SAVE);
    QTextStream stream(&file);

    file.open(QIODevice::WriteOnly);

    foreach(QString line, toList()){
        stream << line << "\n";
    }

    file.close();
}

void UserList::load()
{
    QFile file(USER_SAVE);
    if(file.exists()){

        file.open(QIODevice::ReadOnly);

        QString content = file.readAll();

        foreach(QString line, content.split("\n")){
            QStringList tokens = line.split(":");

            qDebug() << line;

            if(tokens.length() > 1){
                User user;
                user.id = tokens[0].toInt();
                user.name = tokens[1];

                addUser(user.name, user.id);
            }

        }

    }
}

QString UserList::getString(int idx)
{
    QString user;
    user.append(QString("%1").arg(_userList[idx].id));
    user.append(":");
    user.append(_userList[idx].name);

    return user;
}

QStringList UserList::toList()
{
    UserIterator iter;
    QStringList list;

    for(iter = _userList.begin(); iter != _userList.end(); ++iter){

        QString user;
        user.append(QString("%1").arg((*iter).id));
        user.append(":");
        user.append((*iter).name);

        list << user;

    }

    return list;
}

int UserList::getUserId(int idx)
{
    if(idx > 0 && idx < _userList.size()){
        return _userList[idx].id;
    }
    return -1;
}

QString UserList::getLast()
{
    return getString(_userList.size()-1);
}

int UserList::size() const
{
    return _userList.size();
}

