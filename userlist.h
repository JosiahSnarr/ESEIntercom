#ifndef USERLIST_H
#define USERLIST_H

#include <QObject>
#include <vector>

#define USER_SAVE "users.json"

/**
    Manage database of users
*/
class UserList : public QObject
{
    Q_OBJECT
public:
    explicit UserList(QObject *parent = 0);

    //! simple user definition
    struct User{
        QString name;
        int id;
    };
    typedef std::vector<User>::iterator UserIterator;

    bool addUser(QString name, int id);
    void save();
    void load();

    int getUserId(unsigned int idx);

    QString getString(int idx);
    QStringList toList();
    QString getLast();

    int size() const;

private:
    //! list of users
    std::vector<User> _userList;

};

#endif // USERLIST_H
