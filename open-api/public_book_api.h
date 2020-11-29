#ifndef PUBLIC_BOOK_API_H
#define PUBLIC_BOOK_API_H

#pragma once
#include "http_utils.h"
#include "json_utils.h"

namespace BookPublicApi
{
    vector<Book> searchBook(const QString & search);
    QByteArray searchPicture(int id);
}

#endif // PUBLIC_BOOK_API_H
