#include <vector>
#pragma once
#include "storage.h"
#include "http_utils.h"
#include "json_utils.h"
#include <QByteArray>
#include <QPixmap>
#include <QBuffer>


class BookServerApi : public Storage
{
   const QString server_url;
   QNetworkAccessManager manager;

public:

   explicit BookServerApi(const QString & server_url);
   // API
   bool isOpen() const;
   bool exists() const;
   bool open();
   void close();

   // books
   vector<Book> getAllBooks(void);
   optional<Book> getBookById(int book_id);
   bool updateBook(const Book &book_);
   bool removeBook(int book_id);
   int insertBook(const Book &book_);

   // authors
   vector<Author> getAllAuthors(void);
   optional<Author> getAuthorById(int author_id);
   bool updateAuthor(const Author &author_);
   bool removeAuthor(int author_id);
   int insertAuthor(const Author &author_);

   // users
   bool registration_user(QString & username, QString & password);
   bool change_user_options(User & user_, QString & username, QString & password);
   optional<User> getUserAuth(QString & username, QString & password);

   //links books
   vector<Book> getAllUserBooks(int user_id, int page);
   bool insertBookToUser(int book_id, int user_id);
   bool removeBookFromUser(int book_id, int user_id);
   optional<Book> getUserBook(int book_id, int user_id);

   //links author
   vector<Author> getAllBooksAuthors(int book_id);
   bool insertBookAuthor(int book_id, int author_id);
   bool removeBookAuthor(int book_id, int author_id);
   optional<Author> getBookAuthor(int book_id, int author_id);

   //pictures
   QByteArray getBookPicture(int key);
   bool saveBookPicture(int key, QByteArray &picture_);

   QByteArray getAvatar(int key);
   bool saveAvatar(int key, QByteArray &picture_);

   bool check_connection();
   int getPagesCount(int user_id);
};

QByteArray convertMapToByte(QPixmap & pict);

