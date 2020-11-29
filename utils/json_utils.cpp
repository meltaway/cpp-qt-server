#include "json_utils.h"

QString jsonObjectToString(const QJsonObject & jobj)
{
   QJsonDocument doc;
   doc.setObject(jobj);
   return doc.toJson();
}
QString jsonArrayToString(const QJsonArray & jarr)
{
   QJsonDocument doc;
   doc.setArray(jarr);
   return doc.toJson();
}
QString stringToJson(const QString & str)
{
   QJsonParseError err;
   QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &err);
   if (err.error != QJsonParseError::NoError) { throw err; }
   QString qStr = doc.toJson(QJsonDocument::JsonFormat::Indented);
   return qStr;
}

QJsonDocument stringToJsonDocument(const QString & str)
{
   QJsonParseError err;
   QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &err);
   if (err.error != QJsonParseError::NoError) { throw err; }
   return doc;
}


//books

QJsonObject bookToJsonObject(const Book & book_)
{
    QJsonObject jobj;
    jobj.insert("title", book_.title);
    jobj.insert("id", book_.id);
    jobj.insert("genre", book_.genre);
    jobj.insert("creator_id", book_.creator_id);
    jobj.insert("description", book_.description);
    jobj.insert("ISBN_code", book_.ISBN_code);
    jobj.insert("number_of_pages", book_.number_of_pages);
    jobj.insert("original_language", book_.original_language);
    jobj.insert("posted", book_.posted);
    jobj.insert("publication_date", book_.publication_date);
    jobj.insert("rating_count", book_.rating_count);
    jobj.insert("rating_sum", book_.rating_sum);
    jobj.insert("interesting_facts", book_.interesting_facts);
    return jobj;
}

Book jsonObjectToBook(const QJsonObject & jobj)
{
   Book book_;
   book_.creator_id = jobj.value("creator_id").toInt();
   book_.description = jobj.value("description").toString();
   book_.genre = jobj.value("genre").toString();
   book_.id = jobj.value("id").toInt();
   book_.interesting_facts = jobj.value("interesting_facts").toString();
   book_.ISBN_code = jobj.value("ISBN_code").toString();
   book_.number_of_pages = jobj.value("number_of_pages").toInt();
   book_.original_language = jobj.value("original_language").toString();
   book_.posted = jobj.value("posted").toBool();
   book_.publication_date = jobj.value("publication_date").toString();
   book_.rating_count = jobj.value("rating_count").toInt();
   book_.rating_sum = jobj.value("rating_sum").toInt();
   book_.title = jobj.value("title").toString();
   return book_;
}

vector<Book> parseJsonArraytoBooks(QString & jsonText)
{
    vector<Book> books;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() <<"JSON text parsing error: " << err.errorString();
        return books;
    }
    QJsonArray rootArray = doc.array();
    for (QJsonValue jvalue : rootArray)
    {
        QJsonObject jobj = jvalue.toObject();
        Book book_ = jsonObjectToBook(jobj);
        books.push_back(book_);
    }
    return books;
}

QString formatBookstoJson(vector<Book> books_)
{
   QJsonDocument doc;
   QJsonArray booksArray;
   for (const Book & book_: books_)
   {
       QJsonObject stObj = bookToJsonObject(book_);
       booksArray.push_back(stObj);
   }
   doc.setArray(booksArray);

   QString qStr = doc.toJson(QJsonDocument::JsonFormat::Indented);
   return qStr;
}

//users


User jsonObjectToUser(const QJsonObject & jobj)
{
    User user_;
    user_.id = jobj.value("id").toInt();
    user_.username = jobj.value("username").toString();
    user_.password_hash = jobj.value("password_hash").toString();
    user_.tg_teg = jobj.value("tg_teg").toString();

    return user_;
}

QJsonObject userToJsonObject(const User & user)
{
    QJsonObject jobj;
    jobj.insert("username", user.username);
    jobj.insert("password_hash", user.password_hash);
    jobj.insert("tg_teg", user.tg_teg);
    jobj.insert("id", user.id);
    return jobj;
}

//authors

QJsonObject authorToJsonObject(const Author & author_)
{
    QJsonObject jobj;
    jobj.insert("name", author_.name);
    jobj.insert("id", author_.id);
    jobj.insert("birthday_date", author_.birthday_date);
    return jobj;
}

QString formatAuthorstoJson(vector<Author> authors_)
{
    QJsonDocument doc;
    QJsonArray authorsArray;
    for (auto item: authors_)
    {
        QJsonObject stObj = authorToJsonObject(item);
        authorsArray.push_back(stObj);
    }
    doc.setArray(authorsArray);

    QString qStr = doc.toJson(QJsonDocument::JsonFormat::Indented);
    return qStr;
}

Author jsonObjectToAuthor(const QJsonObject & jobj)
{
    Author author_;
    author_.name = jobj.value("name").toString();
    author_.birthday_date = jobj.value("birthday_date").toString();
    author_.id = jobj.value("id").toInt();
    return author_;
}

vector<Author> parseJsonArraytoAuthors(QString & jsonText)
{
    vector<Author> authors;
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError)
    {
        qDebug() <<"JSON Authors vector parsing error: " << err.errorString();
        return authors;
    }
    QJsonArray rootArray = doc.array();
    for (QJsonValue jvalue : rootArray)
    {
        QJsonObject jobj = jvalue.toObject();
        Author author_ = jsonObjectToAuthor(jobj);
        authors.push_back(author_);
    }
    return authors;
}
