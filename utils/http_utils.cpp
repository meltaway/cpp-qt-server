#include "http_utils.h"
#include <QEventLoop>

QString http::httpGet(QNetworkAccessManager & manager, const QString & url)
{
   QNetworkRequest request{url};
   QEventLoop wait;
   QNetworkReply *reply = manager.get(request);
   wait.connect(&manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
   wait.exec();
   QString replyBodyString = QString::fromUtf8(reply->readAll());
   QNetworkReply::NetworkError error = reply->error();
   reply->deleteLater();
   if (error != QNetworkReply::NetworkError::NoError)
       throw NetworkException(error, replyBodyString);
   return replyBodyString;
}

QString http::httpPost(QNetworkAccessManager & manager, const QString & url, const QString & content_type, const QString & body)
{
   QNetworkRequest request{url};
   request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, content_type);
   QEventLoop wait;
   QNetworkReply *reply = manager.post(request, body.toUtf8());
   wait.connect(&manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
   wait.exec();
   QString replyBodyString = QString::fromUtf8(reply->readAll());
   QNetworkReply::NetworkError error = reply->error();
   reply->deleteLater();
   if (error != QNetworkReply::NetworkError::NoError)
       throw NetworkException(error, replyBodyString);
   return replyBodyString;
}

QString http::httpPut(QNetworkAccessManager & manager, const QString & url, const QString & content_type, const QString & body)
{
   QNetworkRequest request{url};
   request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, content_type);
   QEventLoop wait;
   QNetworkReply *reply = manager.put(request, body.toUtf8());
   wait.connect(&manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
   wait.exec();
   QString replyBodyString = QString::fromUtf8(reply->readAll());
   QNetworkReply::NetworkError error = reply->error();
   reply->deleteLater();
   if (error != QNetworkReply::NetworkError::NoError)
       throw NetworkException(error, replyBodyString);
   return replyBodyString;
}

QString http::httpDelete(QNetworkAccessManager & manager, const QString & url)
{
   QNetworkRequest request{url};
   QEventLoop wait;
   QNetworkReply *reply = manager.deleteResource(request);
   wait.connect(&manager, SIGNAL(finished(QNetworkReply*)), &wait, SLOT(quit()));
   wait.exec();
   QString replyBodyString = QString::fromUtf8(reply->readAll());
   QNetworkReply::NetworkError error = reply->error();
   reply->deleteLater();
   if (error != QNetworkReply::NetworkError::NoError)
       throw NetworkException(error, replyBodyString);
   return replyBodyString;
}
