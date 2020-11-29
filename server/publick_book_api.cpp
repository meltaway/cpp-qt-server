#include "publick_book_api.h"

Book BookPublicApi::searchBook(const QString & search)
{
   QString url = QString{"http://openlibrary.org/search.json?title=%1&limit=1"}.arg(search);
   QString res = httpGet(manager, url);

   QJsonArray docs = stringToJsonDocument(res).object().value("docs").toArray();
   qDebug() << "# Book Results: " << docs.count();
   //
   QJsonObject jbook = docs[0].toObject();
   int cover_i = jbook.value("cover_i").toInt();
   OlBook book;
   book.cover_url = QString{"http://covers.openlibrary.org/b/id/%1.jpg"}.arg(QString::number(cover_i));
   book.title = jbook.value("title").toString();
   book.author_name = jbook.value("author_name").toArray()[0].toString();
   return book;
}

QString BookPublicApi::httpGet(QNetworkAccessManager & manager, const QString & url)
{
   QNetworkRequest request;
   qDebug() << "# GET " << url;
   request.setUrl(QUrl(url));
   NetworkWaiter waiter{manager};
   QNetworkReply *reply = manager.get(request);
   qDebug() << "# waiting for response...";
   waiter.wait();
   qDebug() << "# got response...";
   QByteArray replyBodyBytes = reply->readAll();
   QString replyBodyString = QString::fromUtf8(replyBodyBytes);
   QNetworkReply::NetworkError error = reply->error();
   reply->deleteLater();  //  delete reply;
   if (error != QNetworkReply::NetworkError::NoError)
       throw NetworkException(error, replyBodyString);
   return replyBodyString;
}
