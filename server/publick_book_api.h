#ifndef PUBLICK_BOOK_API_H
#define PUBLICK_BOOK_API_H

#pragma once
#include <QNetworkAccessManager>
#include <QException>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include "json_utils.h"

namespace BookPublicApi
{
    Book OpenLibraryApi::searchBook(const QString & search);
    QString httpGet(QNetworkAccessManager & manager, const QString & url);
}
#endif // PUBLICK_BOOK_API_H
