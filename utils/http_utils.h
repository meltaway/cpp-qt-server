#pragma once
#include <QNetworkAccessManager>
#include <QException>
#include <QNetworkReply>
#include <QCoreApplication>

class NetworkException : QException
{
public:
   const QNetworkReply::NetworkError error;
   const QString message;

   NetworkException(
       QNetworkReply::NetworkError error,
       const QString & message): error{error},message{message}
       {
       }
};

namespace http
{
    QString httpGet(QNetworkAccessManager & manager, const QString & url);
    QString httpPost(QNetworkAccessManager & manager, const QString & url, const QString & content_type, const QString & body);
    QString httpPut(QNetworkAccessManager & manager, const QString & url, const QString & content_type, const QString & body);
    QString httpDelete(QNetworkAccessManager & manager, const QString & url);
}
