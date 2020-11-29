#include "public_book_api.h"

QString httpGet(const QString & url)
{
    try
    {
        QNetworkAccessManager manager;
        QNetworkRequest request;
        //qDebug() << "# GET " << url;
        request.setUrl(QUrl(url));
        QEventLoop wait;
        QNetworkReply *reply = manager.get(request);
        //qDebug() << "# waiting for response...";
        wait.connect(&manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
        wait.exec();
        //qDebug() << "# got response...";
        QByteArray replyBodyBytes = reply->readAll();
        QString replyBodyString = QString::fromUtf8(replyBodyBytes);
        QNetworkReply::NetworkError error = reply->error();
        reply->deleteLater();  //  delete reply;
        if (error != QNetworkReply::NetworkError::NoError)
           throw NetworkException(error, replyBodyString);
        return replyBodyString;
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API: " << nex.error << " | " << nex.message;
    }
}

QByteArray httpGetRaw(QNetworkAccessManager & manager, const QString & url)
{
    try
    {
        QNetworkRequest request;
        request.setUrl(QUrl(url));
        QEventLoop wait;
        QNetworkReply *reply = manager.get(request);
        wait.connect(&manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
        wait.exec();

        QString redirect_target = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        QString raw_content_length = reply->header(QNetworkRequest::KnownHeaders::ContentLengthHeader).toString();
        QString transfer_encoding = reply->rawHeader("Transfer-Encoding");

        QByteArray replyBodyBytes;
        if (!redirect_target.isEmpty())
        {
           replyBodyBytes = httpGetRaw(manager, redirect_target);
        }
        else if (!raw_content_length.isEmpty())
        {
           int content_length = raw_content_length.toInt();
           replyBodyBytes = reply->readAll();
           while (replyBodyBytes.length() != content_length)
           {
               wait.exec();
               replyBodyBytes.append(reply->readAll());
           }
        }
        else if (!transfer_encoding.isEmpty() && transfer_encoding == "chunked")
        {
           replyBodyBytes = reply->readAll();
        }

        QNetworkReply::NetworkError error = reply->error();
        reply->deleteLater();  //  delete reply;
        if (error != QNetworkReply::NetworkError::NoError)
           throw NetworkException(error, QString::fromUtf8(replyBodyBytes));
        return replyBodyBytes;
    }
    catch (const NetworkException & nex)
    {
        qDebug() << "Error connecting to API: " << nex.error << " | " << nex.message;
    }
}

vector<Book> BookPublicApi::searchBook(const QString & search)
{
   QString url = QString{"http://openlibrary.org/search.json?title=%1&limit=15"}.arg(search);
   QString res = httpGet(url);

   QJsonArray docs = stringToJsonDocument(res).object().value("docs").toArray();
   //qDebug() << "# Book Results: " << docs.count();
   //
   vector<Book> books;
   for (auto item: docs)
   {
        QJsonObject jbook = item.toObject();
        Book book;
        book.id = jbook.value("cover_i").toInt();
        book.title = jbook.value("title").toString();
        QJsonArray authors = jbook.value("author_name").toArray();
        vector<Author> tmpA;
        for (int i = 0; i < authors.size(); i++)
        {
            Author buffer_author;
            buffer_author.name = authors[i].toString();
            tmpA.push_back(buffer_author);
        }
        book.authors_ = tmpA;
        book.number_of_pages = jbook.value("number_of_pages").toInt();
        book.ISBN_code = jbook.value("isbn").toArray()[0].toString();
        book.publication_date = QString::number(jbook.value("first_publish_year").toInt());
        book.genre = jbook.value("subject").toArray()[0].toString();
        book.original_language = jbook.value("language").toArray()[0].toString();
        books.push_back(book);
   }
   return books;
}

QByteArray BookPublicApi::searchPicture(int id)
{
    QString url = QString{"http://covers.openlibrary.org/b/id/%1.jpg"}.arg(QString::number(id));
    QNetworkAccessManager manager;
    return httpGetRaw(manager, url);
}
