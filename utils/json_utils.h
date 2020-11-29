#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <vector>
#include "storage.h"



QString jsonObjectToString(const QJsonObject & jobj);
QString jsonArrayToString(const QJsonArray & jarr);
QString stringToJson(const QString & str);
QJsonDocument stringToJsonDocument(const QString & str);

//books
QJsonObject bookToJsonObject(const Book & book_);
Book jsonObjectToBook(const QJsonObject & jobj);

vector<Book> parseJsonArraytoBooks(QString & jsonText);
QString formatBookstoJson(vector<Book> books_);

//users

User jsonObjectToUser(const QJsonObject & jobj);
QJsonObject userToJsonObject(const User & user);

//authors

QJsonObject authorToJsonObject(const Author & author_);
QString formatAuthorstoJson(vector<Author> authors_);
Author jsonObjectToAuthor(const QJsonObject & jobj);
vector<Author> parseJsonArraytoAuthors(QString & jsonText);
