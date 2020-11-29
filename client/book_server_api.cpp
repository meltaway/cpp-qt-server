#include "book_server_api.h"
#include <QDebug>

using namespace http;

BookServerApi::BookServerApi(const QString & server_url):server_url{server_url}
{
}

bool BookServerApi::isOpen() const{}
bool BookServerApi::exists() const{}
bool BookServerApi::open(){}
void BookServerApi::close(){}

// books
vector<Book> BookServerApi::getAllBooks(void)
{
    vector<Book> books_;
    try
    {
        QString url = server_url + "/getAllBooks";
        QString res_str = httpGet(manager, url);
        books_ = parseJsonArraytoBooks(res_str);
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API server: " << nex.error << " | " << nex.message;
    }
    return books_;
}
optional<Book> BookServerApi::getBookById(int book_id)
{
    try
    {
        QString url = server_url + "/getBooksByID/" + QString::number(book_id);
        QString res_str = httpGet(manager, url);
        return jsonObjectToBook(stringToJsonDocument(res_str).object());
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API server: " << nex.error << " | " << nex.message;
        return nullopt;
    }
}
bool BookServerApi::updateBook(const Book &book_)
{
    try
    {
       QString url = server_url + "/updateBook";
       QString body = jsonObjectToString(bookToJsonObject(book_));
       httpPut(manager, url, "text/json", body);
       return true;
    }
    catch (const NetworkException & nex)
       { return false; }
}

bool BookServerApi::removeBook(int book_id)
{
    QString url = server_url + "/deleteBook/" + QString::number(book_id);
    try
    {
          httpDelete(manager, url);
          return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

int BookServerApi::insertBook(const Book &book_)
{
    try
    {
       QString url = server_url + "/insertBook";
       QString body = jsonObjectToString(bookToJsonObject(book_));
       QString res_str = httpPost(manager, url, "text/json", body);
       Book new_book = jsonObjectToBook(stringToJsonDocument(res_str).object());
       return new_book.id;
    }
    catch (const NetworkException & nex)
       { return -1; }
}

// authors
vector<Author> BookServerApi::getAllAuthors(void)
{
    vector<Author> authors_;
    try
    {
        QString url = server_url + "/getAllAuthors";
        QString res_str = httpGet(manager, url);
        authors_ = parseJsonArraytoAuthors(res_str);
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API server: " << nex.error << " | " << nex.message;
    }
    return authors_;
}
optional<Author> BookServerApi::getAuthorById(int author_id)
{
    try
    {
        QString url = server_url + "/getAuthorById/" + QString::number(author_id);
        QString res_str = httpGet(manager, url);
        return jsonObjectToAuthor(stringToJsonDocument(res_str).object());
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API server: " << nex.error << " | " << nex.message;
        return nullopt;
    }
}
bool BookServerApi::updateAuthor(const Author &author_){}
bool BookServerApi::removeAuthor(int author_id){}
int BookServerApi::insertAuthor(const Author &author_)
{
    try
    {
       QString url = server_url + "/insertAuthor";
       QString body = jsonObjectToString(authorToJsonObject(author_));
       QString res_str = httpPost(manager, url, "text/json", body);
       Author new_author = jsonObjectToAuthor(stringToJsonDocument(res_str).object());
       return new_author.id;
    }
    catch (const NetworkException & nex)
       { return -1; }
}

// users
bool BookServerApi::registration_user(QString & username, QString & password)
{
    try
    {
        QString url = server_url + "/newUser/" + username + "&" + password;
        httpPost(manager, url, "text/json", "");
        return true;
    }
    catch (const NetworkException & nex)
       { return false; }
}

bool BookServerApi::change_user_options(User & user_, QString & username, QString & password)
{
    QString url = server_url + "/updateUser/" + username + "&" + password;
    QString body = jsonObjectToString(userToJsonObject(user_));
    try
    {
       httpPut(manager, url, "text/json", body);
       return true;
    }
    catch (const NetworkException & nex)
    { return false; }
}

optional<User> BookServerApi::getUserAuth(QString & username, QString & password)
{
    try
    {
        QString url = server_url + "/getUser/" + username + "&" + password;
        QString res_str = httpGet(manager, url);
        User user_ = jsonObjectToUser(stringToJsonDocument(res_str).object());
        return user_;
    }
    catch (const NetworkException & nex)
        { return nullopt; }
}

//links books
vector<Book> BookServerApi::getAllUserBooks(int user_id, int page)
{
       vector<Book> user_books;
       try
       {
           QString url = server_url + "/getUserBooks/" + QString::number(user_id) + "&" + QString::number(page);
           QString res_str = httpGet(manager, url);
           user_books = parseJsonArraytoBooks(res_str);
       }
       catch (const NetworkException & nex)
       {
           qDebug() << "Error connecting to API server: " << nex.error << " | " << nex.message;
           return user_books;
       }
       return user_books;
}

bool BookServerApi::insertBookToUser(int book_id, int user_id)
{
    try
       {
           QString url = server_url + "/newUserBook/" + QString::number(user_id) + "&" + QString::number(book_id);
           httpPost(manager, url, "text/json", "");
           return true;
       }
       catch (const NetworkException & nex)
           { return false; }
}

bool BookServerApi::removeBookFromUser(int book_id, int user_id)
{
    QString url = server_url + "/deleteUserBook/" + QString::number(user_id) + "&" + QString::number(book_id);
    try
    {
          httpDelete(manager, url);
          return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

optional<Book> BookServerApi::getUserBook(int book_id, int user_id)
{
    try
    {
        QString url = server_url + "/getUserBook/" + QString::number(user_id) + "&" + QString::number(book_id);
        QString res_str = httpGet(manager, url);
        Book book_ = jsonObjectToBook(stringToJsonDocument(res_str).object());
        return book_;
    }
    catch (const NetworkException & nex)
        { return nullopt; }
}

//links author
vector<Author> BookServerApi::getAllBooksAuthors(int book_id)
{
    vector<Author> book_authors;
    try
    {
        QString url = server_url + "/getBookAuthors/" + QString::number(book_id);
        QString res_str = httpGet(manager, url);
        book_authors = parseJsonArraytoAuthors(res_str);
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API server: " << nex.error << " | " << nex.message;
        return book_authors;
    }
    return book_authors;
}

bool BookServerApi::insertBookAuthor(int book_id, int author_id)
{
    try
    {
        QString url = server_url + "/newBookAuthor/" + QString::number(book_id) + "&" + QString::number(author_id);
        httpPost(manager, url, "text/json", "");
        return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

bool BookServerApi::removeBookAuthor(int book_id, int author_id)
{
    QString url = server_url + "/deleteBookAuthor/" + QString::number(book_id) + "&" + QString::number(author_id);
    try
    {
          httpDelete(manager, url);
          return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

optional<Author> BookServerApi::getBookAuthor(int book_id, int author_id)
{
    try
    {
        QString url = server_url + "/getBookAuthor/" + QString::number(book_id) + "&" + QString::number(author_id);
        QString res_str = httpGet(manager, url);
        Author author_ = jsonObjectToAuthor(stringToJsonDocument(res_str).object());
        return author_;
    }
    catch (const NetworkException & nex)
        { return nullopt; }
}

QByteArray convertMapToByte(QPixmap & pict)
{
    QByteArray inByteArray;
    QBuffer inBuffer( &inByteArray );
    inBuffer.open( QIODevice::WriteOnly );
    pict.save( &inBuffer, "PNG" );
    return inByteArray;
}

//pictures
QByteArray BookServerApi::getBookPicture(int key)                                  //исправить
{
    QByteArray output;
    try
    {
        QString url = server_url + "/getBookPicture/" + QString::number(key);
        return QByteArray::fromBase64(httpGet(manager, url).toUtf8());
    }
    catch (const NetworkException & nex)
    { return output; }
}

bool BookServerApi::saveBookPicture(int key, QByteArray &picture_)
{
    try
    {
        QString url = server_url + "/setBookPicture/" + QString::number(key);
        httpPost(manager, url, "text/json", picture_.toBase64());
        return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

QByteArray BookServerApi::getAvatar(int key)
{
    QByteArray output;
    try
    {
        QString url = server_url + "/getAvatar/" + QString::number(key);
        return QByteArray::fromBase64(httpGet(manager, url).toUtf8());
    }
    catch (const NetworkException & nex)
    { return output; }
}
bool BookServerApi::saveAvatar(int key, QByteArray &picture_)
{
    try
    {
        QString url = server_url + "/setAvatar/" + QString::number(key);
        httpPost(manager, url, "text/json", picture_.toBase64());
        return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

bool BookServerApi::check_connection()
{
    try
    {
        QString url = server_url + "/checkConnection";
        httpGet(manager, url);
        return true;
    }
    catch (const NetworkException & nex)
        { return false; }
}

int BookServerApi::getPagesCount(int user_id)
{
    try
    {
        QString url = server_url + "/checkPagesCount/" + QString::number(user_id);
        return httpGet(manager, url).toInt();
    }
    catch (const NetworkException & nex)
        { return -1; }
}
