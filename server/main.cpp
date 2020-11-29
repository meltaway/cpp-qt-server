#include <QTcpServer>
#include <QTcpSocket>
#include <QDateTime>
#include "server.h"
#include "sqlite_storage.h"
#include "json_utils.h"

void onServerError(QAbstractSocket::SocketError socketError)
{
   qDebug() << "tcp server error: " << socketError;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Storage * storage = new SqliteStorage("/home/bujhmt/progbase3/data_base");
    HttpServer http_server;

//========================================================================= USER-BOOK LINKS

    http_server.get("/getUserBooks/:id&:page", [&](HttpRequest & req, HttpResponse & res)
    {
       int user_id = req.params["id"].toInt();
       int page = req.params["page"].toInt();
       storage->open();
       vector<Book> books = storage->getAllUserBooks(user_id, page);
       storage->close();
       res.setContentType("text/json");
       res.body = formatBookstoJson(books);

       qDebug() << "    << getUserBooks, user - " << user_id <<"    status - " << !books.empty();
    });

    http_server.get("/getUserBook/:user_id&:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int user_id = req.params["user_id"].toInt();
       int book_id = req.params["book_id"].toInt();
       storage->open();
       optional<Book> book = storage->getUserBook(book_id, user_id);
       storage->close();
       if (book)
       {
           res.setContentType("text/json");
           res.body = jsonObjectToString(bookToJsonObject(book.value()));
       }
       else
       {
           res.setStatus(404, "Book not found");
       }

       qDebug() << "    << getUserBook, user - " << user_id << "    status - " << book.has_value();
    });

    http_server.post("/newUserBook/:user_id&:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int user_id = req.params["user_id"].toInt();
       int book_id = req.params["book_id"].toInt();
       storage->open();
       bool correct = storage->insertBookToUser(book_id, user_id);
       storage->close();
       if (!correct)
       {
           res.setStatus(400, "Data base error");
       }
       else
       {
           res.setStatus(200, "Book added to User");
       }

       qDebug() << "    >> newUserBook, user - " << user_id << "    status - " << correct;
    });

    http_server.delete_("/deleteUserBook/:user_id&:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
        int user_id = req.params["user_id"].toInt();
        int book_id = req.params["book_id"].toInt();
        storage->open();
        bool correct = storage->removeBookFromUser(book_id, user_id);
        storage->close();
        if (!correct)
        {
            res.setStatus(400, "Data base error");
        }
        else
        {
            res.setStatus(200, "Book deleted from User");
        }

        qDebug() << "    >> deleteUserBook, user - " << user_id << "    status - " << correct;
    });

//========================================================================= USER

    http_server.post("/newUser/:username&:password", [&](HttpRequest & req, HttpResponse & res)
    {
       QString username = req.params["username"];
       QString password = req.params["password"];
       storage->open();
       bool correct = storage->registration_user(username, password);
       storage->close();
       if (!correct)
       {
           res.setStatus(400, "Data base error");
       }
       else
       {
           res.setStatus(200, "New User added");
       }

       qDebug() << "    >> newUser, user - """ << username << """    status - " << correct;
    });

    http_server.put("/updateUser/:username&:password", [&](HttpRequest & req, HttpResponse & res)
    {
       QString username = req.params["username"];
       QString password = req.params["password"];

       User updated_user = jsonObjectToUser(stringToJsonDocument(req.body).object());

       storage->open();
       bool correct = storage->change_user_options(updated_user, username, password);
       storage->close();
       if (!correct)
       {
           res.setStatus(400, "Data base error");
       }
       else
       {
           res.setStatus(200, "User updated");
       }

       qDebug() << "    >> updateUser, user - """ << username << """    status - " << correct;
    });

    http_server.get("/getUser/:username&:password", [&](HttpRequest & req, HttpResponse & res)
    {
        QString username = req.params["username"];
        QString password = req.params["password"];

        storage->open();
        optional<User> user_opt = storage->getUserAuth(username, password);
        storage->close();
        if (!user_opt)
        {
            res.setStatus(404, "User not found");
        }
        else
        {
            res.setContentType("text/json");
            res.body = jsonObjectToString(userToJsonObject(user_opt.value()));
        }

        qDebug() << "    << getUser, user - """ << username << """    status - " << user_opt.has_value();
    });

    //========================================================================= AUTHOR-BOOK LINKS

    http_server.get("/getBookAuthors/:id", [&](HttpRequest & req, HttpResponse & res)
    {
       int book_id = req.params["id"].toInt();
       storage->open();
       vector<Author> authors = storage->getAllBooksAuthors(book_id);
       storage->close();
       res.setContentType("text/json");
       res.body = formatAuthorstoJson(authors);

       qDebug() << "    << getBookAuthors, book - " << book_id << "    status - " << !authors.empty();
    });

    http_server.post("/newBookAuthor/:book_id&:author_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int author_id = req.params["author_id"].toInt();
       int book_id = req.params["book_id"].toInt();
       storage->open();
       bool correct = storage->insertBookAuthor(book_id, author_id);
       storage->close();
       if (!correct)
       {
           res.setStatus(400, "Data base error");
       }
       else
       {
           res.setStatus(200, "Author added to Book");
       }

       qDebug() << "    >> newBookAuthor, book - " << book_id << " ; author -" << author_id << "    status - " << correct;
    });

    http_server.delete_("/deleteBookAuthor/:book_id&:author_id", [&](HttpRequest & req, HttpResponse & res)
    {
        int author_id = req.params["author_id"].toInt();
        int book_id = req.params["book_id"].toInt();
        storage->open();
        bool correct = storage->removeBookAuthor(book_id, author_id);
        storage->close();
        if (!correct)
        {
            res.setStatus(400, "Data base error");
        }
        else
        {
            res.setStatus(200, "Author deleted from Book");
        }

    qDebug() << "    >> deleteBookAuthor, book - " << book_id << " ; author -" << author_id << "    status - " << correct;
    });

    http_server.get("/getBookAuthor/:book_id&:author_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int author_id = req.params["author_id"].toInt();
       int book_id = req.params["book_id"].toInt();
       storage->open();
       optional<Author> author = storage->getBookAuthor(book_id, author_id);
       storage->close();
       if (author)
       {
           res.setContentType("text/json");
           res.body = jsonObjectToString(authorToJsonObject(author.value()));
       }
       else
       {
           res.setStatus(404, "Book not found");
       }

       qDebug() << "    << getBookAuthor, book - " << book_id << "    status - " << author.has_value();
    });

    //========================================================================= PICTURES

    http_server.get("/getBookPicture/:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int book_id = req.params["book_id"].toInt();

       storage->open();
       QByteArray picture = storage->getBookPicture(book_id);
       storage->close();
       if (!picture.isNull())
       {
           res.setContentType("text/json");
           res.body = picture.toBase64();
       }
       else
       {
           res.setStatus(404, "Book picture not found");
       }

       qDebug() << "    << getBookPicture, book - " << book_id << "    status - " << !picture.isNull();
    });

    http_server.post("/setBookPicture/:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int book_id = req.params["book_id"].toInt();
       QByteArray buf = QByteArray::fromBase64(req.body.toUtf8());
       storage->open();
       bool correct = storage->saveBookPicture(book_id, buf);
       storage->close();
       if (!correct)
       {
           res.setStatus(400, "Data base error");
       }
       else
       {
           res.setStatus(200, "Picture added to Book");
       }

       qDebug() << "    >> setBookPicture, book - " << book_id << "    status - " << correct;
    });

    http_server.get("/getAvatar/:user_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int user_id = req.params["user_id"].toInt();

       storage->open();
       QByteArray picture = storage->getAvatar(user_id);
       storage->close();
       if (!picture.isNull())
       {
           res.setContentType("text/json");
           res.body = picture.toBase64();
       }
       else
       {
           res.setStatus(404, "Avatar not found");
       }

       qDebug() << "    << getAvatar, user - " << user_id << "    status - " << !picture.isNull();
    });

    http_server.post("/setAvatar/:user_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int user_id = req.params["user_id"].toInt();
       QByteArray buf = QByteArray::fromBase64(req.body.toUtf8());
       storage->open();
       bool correct = storage->saveAvatar(user_id, buf);
       storage->close();
       if (!correct)
       {
           res.setStatus(400, "Data base error");
       }
       else
       {
           res.setStatus(200, "Avatar added to user");
       }

       qDebug() << "    >> setAvatar, book - " << user_id << "    status - " << correct;
    });
    //========================================================================= BOOKS

    http_server.post("/insertBook", [&](HttpRequest & req, HttpResponse & res)
    {
       Book new_book = jsonObjectToBook(stringToJsonDocument(req.body).object());
       storage->open();
       new_book.id = storage->insertBook(new_book);
       storage->close();
       res.setContentType("text/json");
       res.body = jsonObjectToString(bookToJsonObject(new_book));

       qDebug() << "    >> insertBook, book - " << new_book.id << "    status - " << (bool)new_book.id;
    });

    http_server.put("/updateBook", [&](HttpRequest & req, HttpResponse & res)
    {
       Book updated_book = jsonObjectToBook(stringToJsonDocument(req.body).object());
       storage->open();
       bool correct = storage->updateBook(updated_book);
       storage->close();
       if (correct)
       {
           res.setStatus(200, "Book updated");
       }
       else
       {
           res.setStatus(400, "Data base error");
       }

       qDebug() << "    >> updateBook, book - " << updated_book.id << "    status - " << correct;
    });

    http_server.get("/getAllBooks", [&](HttpRequest & req, HttpResponse & res)
    {
       storage->open();
       vector<Book> books = storage->getAllBooks();
       storage->close();
       res.setContentType("text/json");
       res.body = formatBookstoJson(books);

       qDebug() << "    << getAllBooks" <<"    status - " << !books.empty();
    });

    http_server.get("/getBooksByID/:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int book_id = req.params["book_id"].toInt();
       storage->open();
       optional<Book> book_opt = storage->getBookById(book_id);
       storage->close();
       if(book_opt)
       {
           res.setContentType("text/json");
           res.body = jsonObjectToString(bookToJsonObject(book_opt.value()));
       }
       else
       {
           res.setStatus(404, "Book not found");
       }

       qDebug() << "    << getBooksByID " << " book_id - " << book_id << "status - " << book_opt.has_value();
    });

    http_server.delete_("/deleteBook/:book_id", [&](HttpRequest & req, HttpResponse & res)
    {
        int book_id = req.params["book_id"].toInt();
        storage->open();
        bool correct = storage->removeBook(book_id);
        storage->close();
        if (!correct)
        {
            res.setStatus(400, "Data base error");
        }
        else
        {
            res.setStatus(200, "Book deleted");
        }

    qDebug() << "    >> deleteBook, book - " << book_id << "    status - " << correct;
    });
//========================================================================= AUTHORS

    http_server.get("/getAllAuthors", [&](HttpRequest & req, HttpResponse & res)
    {
       storage->open();
       vector<Author> authors = storage->getAllAuthors();
       storage->close();
       res.setContentType("text/json");
       res.body = formatAuthorstoJson(authors);

       qDebug() << "    << getAllAuthors" <<"    status - " << !authors.empty();
    });

    http_server.post("/insertAuthor", [&](HttpRequest & req, HttpResponse & res)
    {
       Author new_author = jsonObjectToAuthor(stringToJsonDocument(req.body).object());
       storage->open();
       new_author.id = storage->insertAuthor(new_author);
       storage->close();
       res.setContentType("text/json");
       res.body = jsonObjectToString(authorToJsonObject(new_author));

       qDebug() << "    >> insertAuthor, author - " << new_author.id << "    status - " << (bool)new_author.id;
    });

    http_server.get("/getAuthorById/:id", [&](HttpRequest & req, HttpResponse & res)
    {
       int author_id = req.params["id"].toInt();
       storage->open();
       optional<Author> author_ = storage->getAuthorById(author_id);
       storage->close();
       if (author_.has_value())
       {
           res.setContentType("text/json");
           res.body = jsonObjectToString(authorToJsonObject(author_.value()));
       }
       else
       {
           res.setStatus(404, "Author not found");
       }

       qDebug() << "    << getAuthorById, author - " << author_id << "    status - " << author_.has_value();
    });

    http_server.get("/checkPagesCount/:user_id", [&](HttpRequest & req, HttpResponse & res)
    {
       int user_id = req.params["user_id"].toInt();
       storage->open();
       int count = storage->getPagesCount(user_id);
       storage->close();
       if (count != -1)
       {
           res.setContentType("text/json");
           res.body = QString::number(count);
       }
       else
       {
           res.setStatus(404, "Pages not found");
       }

       qDebug() << "    << checkPagesCount of user - " << user_id << "    pages - " << count;
    });




    http_server.get("/checkConnection", [&](HttpRequest & req, HttpResponse & res)
    {
        res.setStatus(200, "Server rabotaet");
        qDebug() << "    << checkConnection";
    });




    const int port = 3000;
    if (http_server.start(port))
        qDebug() << "started at port " << port;
    else
        qDebug() << "can't start at port " << port;

    return a.exec();
}
