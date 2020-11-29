#include "sqlite_storage.h"
#include <QCryptographicHash>

#define BIND_BOOK_VALUES(query, book_)                                 \
{                                                                      \
query.bindValue(":title", book_.title);                                \
query.bindValue(":description", book_.description);                    \
query.bindValue(":genre", book_.genre);                                \
query.bindValue(":interesting_facts", book_.interesting_facts);        \
query.bindValue(":number_of_pages", book_.number_of_pages);            \
query.bindValue(":ISBN_code", book_.ISBN_code);                        \
query.bindValue(":original_language", book_.original_language);        \
query.bindValue(":publication_date", book_.publication_date);          \
query.bindValue(":rating_sum", book_.rating_sum);                      \
query.bindValue(":rating_count", book_.rating_count);                  \
query.bindValue(":creator_id", book_.creator_id);                      \
query.bindValue(":posted", book_.posted);                              \
};



SqliteStorage::SqliteStorage(const string & dir_name) : Storage(dir_name)
{
    database_ = QSqlDatabase::addDatabase("QSQLITE");
}

bool SqliteStorage::isOpen() const
{
    return database_.isOpen();
}

bool SqliteStorage::exists() const
{
    QFile tmp(QString::fromStdString(name()) + "/data.sqlite");
    return tmp.exists();
}

bool SqliteStorage::open()
{
    if (!name().empty())
    {
        QDir().mkdir(QString::fromStdString(name()));
    }
    else return false;

    database_.setDatabaseName(QString::fromStdString(name()) + "/data.sqlite");

    if (!database_.open())
    {
       qDebug() << "open database error:"
                << database_.lastError();
       return false;
    }
    return true;
}

void SqliteStorage::close()
{
    database_.close();
}

//privat
Book FillBook(QSqlQuery & query)
{
    Book book_;
    book_.title = query.value("title").toString();
    book_.id = query.value("id").toInt();
    book_.description = query.value("description").toString();
    book_.genre = query.value("genre").toString();
    book_.interesting_facts =  query.value("interesting_facts").toString();
    book_.number_of_pages = query.value("number_of_pages").toInt();
    book_.ISBN_code = query.value("ISBN_code").toString();
    book_.original_language = query.value("original_language").toString();
    book_.publication_date = query.value("publication_date").toString();
    book_.rating_sum = query.value("rating_sum").toInt();
    book_.rating_count = query.value("rating_count").toInt();
    book_.creator_id = query.value("creator_id").toInt();
    book_.posted = query.value("posted").toBool();
    return book_;
}


// books
vector<Book> SqliteStorage::getAllBooks(void)
{
    vector<Book> books;
    QSqlQuery query("SELECT * FROM books");
    while (query.next())
    {
       books.push_back(FillBook(query));
    }
    return books;
}

optional<Book> SqliteStorage::getBookById(int book_id)
{
    QSqlQuery query;
    if (!query.prepare("SELECT * FROM books WHERE id = :id"))
    {
       qDebug() << "get book query prepare error:" << query.lastError();
       return nullopt;
    }
    query.bindValue(":id", book_id);
    if (!query.exec())
    {
       qDebug() << "get book query exec error:" << query.lastError();
       return nullopt;
    }
    if (query.next())
    {
        return optional<Book>{FillBook(query)};
    }
    else
    {
       qDebug() << "book not found ";
       return nullopt;
    }
}

bool SqliteStorage::updateBook(const Book &book_)
{
    QSqlQuery query;
    if (!query.prepare("UPDATE books SET title = :title,"
                       " description = :description,"
                       " genre = :genre,"
                       " interesting_facts = :interesting_facts,"
                       " number_of_pages = :number_of_pages,"
                       " ISBN_code = :ISBN_code,"
                       " original_language = :original_language,"
                       " publication_date = :publication_date,"
                       " rating_sum = :rating_sum, "
                       " rating_count = :rating_count,"
                       " creator_id = :creator_id, "
                       " posted = :posted       WHERE id = :id"))
    {
        qDebug() << "updateBook query prepare error:" << query.lastError();
        return false;
    }
    BIND_BOOK_VALUES(query, book_) // bind macros
    query.bindValue(":id", book_.id);
    if (!query.exec())
    {
        qDebug() << "updateBook query exec error:" << query.lastError();
        return 0;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}

bool SqliteStorage::removeBook(int book_id)
{
    QSqlQuery query;
    if (!query.prepare("DELETE FROM books WHERE id = :id"))
    {
        qDebug() << "deleteBook query prepare error:" << query.lastError();
        return false;
    }
    query.bindValue(":id", book_id);
    if (!query.exec())
    {
        qDebug() << "deleteBook query exec error:" << query.lastError();
        return false;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}

int SqliteStorage::insertBook(const Book &book_)
{
    QSqlQuery query;
    if (!query.prepare("INSERT INTO books (title, description, genre,"
                                          "interesting_facts, number_of_pages, ISBN_code,"
                                          "original_language, publication_date, rating_sum, rating_count,"
                                          "creator_id, posted) "
                                  "VALUES (:title, :description, :genre,"
                                          ":interesting_facts, :number_of_pages, :ISBN_code,"
                                          ":original_language, :publication_date, :rating_sum, :rating_count,"
                                          ":creator_id, :posted) "))
    {
        QSqlError error = query.lastError();
        qDebug() << "insertBook prepare error  " << query.lastError();
        throw error;
    }
    BIND_BOOK_VALUES(query, book_)
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        qDebug() << "insertBook exec error  " << query.lastError();
        throw error;
        return 0;
    }
    return query.lastInsertId().toInt();
}


// authors -----------------------------------------------------------------------------------------------------------------------------------------------


#define BIND_AUTHOR_VALUES(query, author_)                            \
{                                                                     \
    query.bindValue(":name", author_.name);                           \
    query.bindValue(":birthday_date", author_.birthday_date);         \
};

Author FillAuthor(QSqlQuery & query)
{
    Author author_;
    author_.birthday_date = query.value("birthday_date").toString();
    author_.id = query.value("id").toInt();
    author_.name = query.value("name").toString();
    return author_;
}

vector<Author> SqliteStorage::getAllAuthors(void)
{
    vector<Author> authors;
    QSqlQuery query("SELECT * FROM authors");
    while (query.next())
    {
       authors.push_back(FillAuthor(query));
    }
    return authors;
}
optional<Author> SqliteStorage::getAuthorById(int author_id)
{
    QSqlQuery query;
    if (!query.prepare("SELECT * FROM authors WHERE id = :id"))
    {
       qDebug() << "get author query prepare error:" << query.lastError();
       return nullopt;
    }
    query.bindValue(":id", author_id);
    if (!query.exec())
    {
       qDebug() << "get author query exec error:" << query.lastError();
       return nullopt;
    }
    if (query.next())
    {
        return optional<Author>{FillAuthor(query)};
    }
    else
    {
       qDebug() << "author not found ";
       return nullopt;
    }
}

bool SqliteStorage::updateAuthor(const Author &author_)
{
    QSqlQuery query;
    if (!query.prepare("UPDATE authors SET name = :name, birthday_date = :birthday_date  WHERE id = :id"))
    {
        qDebug() << "updateAuthor query prepare error:" << query.lastError();
        return false;
    }
    BIND_AUTHOR_VALUES(query, author_) // bind macros
    query.bindValue(":id", author_.id);
    if (!query.exec())
    {
        qDebug() << "updateAuthor query exec error:" << query.lastError();
        return 0;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}

bool SqliteStorage::removeAuthor(int author_id)
{
    QSqlQuery query;
    if (!query.prepare("DELETE FROM authors WHERE id = :id"))
    {
        qDebug() << "deleteAuthor query prepare error:" << query.lastError();
        return false;
    }
    query.bindValue(":id", author_id);
    if (!query.exec())
    {
        qDebug() << "deleteAuthor query exec error:" << query.lastError();
        return false;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}

int SqliteStorage::insertAuthor(const Author &author_)
{
    QSqlQuery query;
    if (!query.prepare("INSERT INTO authors (name, birthday_date) VALUES (:name, :birthday_date)"))
    {
        QSqlError error = query.lastError();
        throw error;
    }
    BIND_AUTHOR_VALUES(query, author_)
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        throw error;
        return 0;
    }
    return query.lastInsertId().toInt();
}


// users -----------------------------------------------------------------------------------------------------------------------------------------------

QString hashPassword(QString const & pass) {
   QByteArray pass_ba = pass.toUtf8();
   QByteArray hash_ba = QCryptographicHash::hash(pass_ba, QCryptographicHash::Md5);
   QString pass_hash = QString(hash_ba.toHex());
   return pass_hash;
}


bool SqliteStorage::registration_user(QString & username, QString & password)
{
    QSqlQuery query;
       if (!query.prepare("SELECT * FROM users "
                          "WHERE username = :username;"))
       {
           QSqlError error = query.lastError();
           throw error;
       }
       query.bindValue(":username", username);
       if (!query.exec())
       {
           QSqlError error = query.lastError();
           throw error;
       }
       if (query.next()) return false;



       if (!query.prepare("INSERT INTO users (username, password_hash) VALUES (:username, :password_hash)"))
       {
           QSqlError error = query.lastError();
           throw error;
       }
       query.bindValue(":username", username);                               \
       query.bindValue(":password_hash", hashPassword(password));
       if (!query.exec())
       {
           QSqlError error = query.lastError();
           throw error;
           return false;
       }
       return true;
}

bool SqliteStorage::change_user_options(User & user_, QString & username, QString & password)
{
    QSqlQuery query;

    if (!query.prepare("SELECT * FROM users "
                       "WHERE username = :username;"))
    {
        QSqlError error = query.lastError();
        throw error;
    }
    query.bindValue(":username", username);
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        throw error;
    }
    if (query.next())
         if (query.value("username").toString() == username && query.value("id").toInt() != user_.id) return false;

    QString sqltext;
    if (!password.isEmpty() && !username.isEmpty()) sqltext = "UPDATE users SET username = :username, password_hash = :password_hash, time_format = :time_format  WHERE id = :id";
    else
    if (password.isEmpty()) sqltext = "UPDATE users SET username = :username, time_format = :time_format WHERE id = :id";
    else
    if (username.isEmpty()) sqltext = "UPDATE users SET password_hash = :password_hash, time_format = :time_format WHERE id = :id";
    if (!query.prepare(sqltext))
    {
        qDebug() << "updateAuthor query prepare error:" << query.lastError();
        return false;
    }
    if (!username.isEmpty()) query.bindValue(":username", username);
    if (!password.isEmpty()) query.bindValue(":password_hash", hashPassword(password));
    query.bindValue(":id", user_.id);
    if (!query.exec())
    {
        qDebug() << "updateAuthor query exec error:" << query.lastError();
        return 0;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}

optional<User> SqliteStorage::getUserAuth(QString & username, QString & password)
{
    QSqlQuery query;
       if (!query.prepare("SELECT * FROM users "
                          "WHERE username = :username AND password_hash = :password_hash;"))
       {
           QSqlError error = query.lastError();
           throw error;
       }
       query.bindValue(":username", username);
       query.bindValue(":password_hash", hashPassword(password));
       if (!query.exec())
       {
           QSqlError error = query.lastError();
           throw error;
       }
       if (query.next())
       {
           User user;
           user.id = query.value("id").toInt();
           user.tg_teg = query.value("tg_teg").toString();
           user.username = username;
           return user;
       }
       return nullopt;
}

//links books

vector<Book> SqliteStorage::getAllUserBooks(int user_id, int page)
{
    vector<Book> books;
    QSqlQuery query;
    QString script;
    if (page == 0) script = "SELECT * FROM book_links WHERE user_id = :user_id";
    else script = "SELECT * FROM book_links WHERE user_id = :user_id LIMIT 10 OFFSET :skipped_items";
    if(!query.prepare(script))
    {
        qDebug() << "Prepare getAllUserBooks error" << query.lastError();
    }
    query.bindValue(":user_id", user_id);
    if (page != 0) query.bindValue(":skipped_items", (page - 1) * 10);
    query.exec();
    while (query.next())
    {
       optional<Book> tmp = getBookById(query.value("book_id").toInt());
       if (tmp) books.push_back(tmp.value());
    }
    return books;
}

bool SqliteStorage::insertBookToUser(int book_id, int user_id)
{
    QSqlQuery query;
    if (!query.prepare("INSERT INTO book_links (book_id, user_id) VALUES (:book_id, :user_id)"))
    {
        QSqlError error = query.lastError();
        throw error;
    }
    query.bindValue(":book_id", book_id);
    query.bindValue(":user_id", user_id);
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        throw error;
        return false;
    }
    return true;
}

bool SqliteStorage::removeBookFromUser(int book_id, int user_id)
{
    QSqlQuery query;
    if (!query.prepare("DELETE FROM book_links WHERE book_id = :book_id AND user_id = :user_id"))
    {
        qDebug() << "removeBookFromUser query prepare error:" << query.lastError();
        return false;
    }
    query.bindValue(":book_id", book_id);
    query.bindValue(":user_id", user_id);
    if (!query.exec())
    {
        qDebug() << "removeBookFromUser query exec error:" << query.lastError();
        return false;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "removeBookFromUser: Zero rows Affected";
        return false;
    }
    return true;
}

optional<Book> SqliteStorage::getUserBook(int book_id, int user_id)
{
    QSqlQuery query;
    if(!query.prepare("SELECT * FROM book_links WHERE book_id = :book_id AND user_id = :user_id"))
    {
        qDebug() << "Prepare userBookExec error" << query.lastError();
        return nullopt;
    }
    query.bindValue(":book_id", book_id);
    query.bindValue(":user_id", user_id);
    query.exec();
    if (query.next())
    {
        return getBookById(book_id);
    }
    else return nullopt;
}


 //links authors

vector<Author> SqliteStorage::getAllBooksAuthors(int book_id)
{
    vector<Author> authors;
    QSqlQuery query;
    if(!query.prepare("SELECT * FROM links WHERE book_id = :book_id"))
    {
        qDebug() << "Prepare getAllBooksAuthors error" << query.lastError();
    }
    query.bindValue(":book_id", book_id);
    query.exec();
    while (query.next())
    {
       authors.push_back(getAuthorById(query.value("author_id").toInt()).value());
    }
    return authors;
}

bool SqliteStorage::insertBookAuthor(int book_id, int author_id)
{
    QSqlQuery query;
    if (!query.prepare("INSERT INTO links (book_id, author_id) VALUES (:book_id, :author_id)"))
    {
        QSqlError error = query.lastError();
        throw error;
    }
    query.bindValue(":book_id", book_id);
    query.bindValue(":author_id", author_id);
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        throw error;
        return false;
    }
    return true;

}
bool SqliteStorage::removeBookAuthor(int book_id, int author_id)
{
    QSqlQuery query;
    if (!query.prepare("DELETE FROM links WHERE book_id = :book_id AND author_id = :author_id"))
    {
        qDebug() << "deleteBookAuthor query prepare error:" << query.lastError();
        return false;
    }
    query.bindValue(":book_id", book_id);
    query.bindValue(":author_id", author_id);
    if (!query.exec())
    {
        qDebug() << "deleteBookAuthor query exec error:" << query.lastError();
        return false;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "deleteBookAuthor: Zero rows Affected";
        return false;
    }
    return true;
}

optional<Author> SqliteStorage::getBookAuthor(int book_id, int author_id)
{
    QSqlQuery query;
    if(!query.prepare("SELECT * FROM links WHERE book_id = :book_id AND author_id = :author_id"))
    {
        qDebug() << "Prepare authorBookExec error" << query.lastError();
        return nullopt;
    }
    query.bindValue(":book_id", book_id);
    query.bindValue(":author_id", author_id);
    query.exec();
    if (query.next()) return getAuthorById(author_id);
    else return nullopt;
}


//pictures


QByteArray SqliteStorage::getBookPicture(int id)
{
    QSqlQuery query;
    if (!query.prepare("SELECT picture FROM books WHERE id = :id"))
    {
        QSqlError error = query.lastError();
        throw error;
    }
    query.bindValue(":id", id);
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        throw error;
    }
    if (query.next())
    {
        return query.value("picture").toByteArray();
    }
}

bool SqliteStorage::saveBookPicture(int id, QByteArray &picture_)
{
    QSqlQuery query;
    if (!query.prepare("UPDATE books SET picture = :picture WHERE id = :id"))
    {
        qDebug() << "save book picture error:" << query.lastError();
        return false;
    }
    query.bindValue(":picture", picture_);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << "save book picture exec error:" << query.lastError();
        return false;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}

QByteArray SqliteStorage::getAvatar(int id)
{
    QSqlQuery query;
    if (!query.prepare("SELECT avatar FROM users WHERE id = :id"))
    {
        QSqlError error = query.lastError();
        throw error;
    }
    query.bindValue(":id", id);
    if (!query.exec())
    {
        QSqlError error = query.lastError();
        throw error;
    }
    if (query.next())
    {
        return query.value("avatar").toByteArray();
    }
}


bool SqliteStorage::saveAvatar(int id, QByteArray &picture_)
{
    QSqlQuery query;
    if (!query.prepare("UPDATE users SET avatar = :avatar WHERE id = :id"))
    {
        qDebug() << "save avatar error:" << query.lastError();
        return false;
    }
    query.bindValue(":avatar", picture_);
    query.bindValue(":id", id);
    if (!query.exec())
    {
        qDebug() << "save avatar exec error:" << query.lastError();
        return false;
    }
    if(query.numRowsAffected() == 0)
    {
        qDebug() << "Zero rows Affected";
        return false;
    }
    return true;
}
int SqliteStorage::getPagesCount(int user_id)
{
    QSqlQuery query;
    QString script;

    if (user_id == -1) script = "SELECT COUNT(*) AS count_ FROM books WHERE posted = 1";
    else script = "SELECT COUNT(*) AS count_ FROM book_links WHERE user_id = :user_id";
    if (!query.prepare(script))
    {
        qDebug() << "getPagesCount error:" << query.lastError();
        return -1;
    }
    if(user_id != -1) query.bindValue(":user_id", user_id);
    if (!query.exec())
    {
        qDebug() << "getPagesCount exec error:" << query.lastError();
        return -1;
    }
    query.next();
    return query.value("count_").toInt();
}

bool SqliteStorage::check_connection(){}
