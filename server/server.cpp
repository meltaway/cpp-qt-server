#include "server.h"
#include <QTextStream>
#include <QUrlQuery>

QMap<QString, QString> parseUrlQuery(const QString & uri)
{
   QMap<QString, QString> query;
   if (uri.contains('?'))
   {
       QUrlQuery url_query{uri.split("?")[1]};
       QList<QPair<QString, QString>> pairs = url_query.queryItems();
       for (QPair<QString, QString> & pair: pairs)
           query[pair.first] = pair.second;
   }
   return query;
}

bool patternMatch(const QString & pattern, const QString & path, QMap<QString, QString> & params)
{
   const QRegExp paramNameRegExp{":\\w+"};
   QRegExp paramNamesRegExp{QString{pattern}.replace(paramNameRegExp, ":([^/]+)")};
   QRegExp paramValuesRegExp{QString{pattern}.replace(paramNameRegExp, "([^/]+)")};
   if (!paramValuesRegExp.exactMatch(path))
       return false;
   paramNamesRegExp.exactMatch(pattern);
   for (int i = 0; i < paramNamesRegExp.captureCount(); i++) {
      QString paramName = paramNamesRegExp.cap(i + 1);
      QString paramValue = paramValuesRegExp.cap(i + 1);
      params[paramName] = paramValue;
   }
   return true;
}


const char * HttpContentTypeHeader = "Content-Type";
const char * HttpContentLengthHeader = "Content-Length";

QString HttpRequest::contentType() const
{
   if (!headers.contains(HttpContentTypeHeader))
       return "";
   return headers[HttpContentTypeHeader];
}
int HttpRequest::contentLength() const
{
   if (!headers.contains(HttpContentLengthHeader))
       return 0;
   return headers[HttpContentLengthHeader].toInt();
}

void HttpResponse::setContentType(const QString & type)
{
   headers[HttpContentTypeHeader] = type;
}
void HttpResponse::setContentLength(int length)
{
   headers[HttpContentLengthHeader] = QString::number(length);
}

void HttpResponse::setStatus(int code, const QString & description)
{
   status_code = code;
   status_description = description;
}





HttpServer::HttpServer(QObject * parent): QObject(parent)
{
   QObject::connect(&tcp_server, &QTcpServer::acceptError,
       this, &HttpServer::onError);
   QObject::connect(&tcp_server, &QTcpServer::newConnection,
       this, &HttpServer::onNewConnection);
}

HttpRequest parseHttpRequest(const QString & str)
{
   HttpRequest res;
   QString copy = str;
   QTextStream ts{&copy};
   // GET /uri HTTP/1.1
   QString request_line = ts.readLine();
   QStringList request_line_parts = request_line.split(" ");
   res.method = request_line_parts[0];
   res.uri = request_line_parts[1];
   res.http_version = request_line_parts[2];
   // Content-type: text/html
   QString header_line;
   while ((header_line = ts.readLine()).length() > 0)
   {
       QStringList parts = header_line.split(": ");
       res.headers[parts[0]] = parts[1];
   }
   //
   res.body = ts.readAll();
   return res;
}

QString formatHttpResponse(const HttpResponse & res)
{
   QString str;
   QTextStream ts{&str};
   // HTTP/1.1 200 OK
   ts << res.http_version << " "
      << res.status_code << " "
      << res.status_description << "\r\n";
   // Content-type: text/html
   QMap<QString, QString>::const_iterator it;
   for (it = res.headers.cbegin(); it != res.headers.cend(); ++it)
       ts << it.key() + ": " + it.value() << "\r\n";
   ts << "\r\n";
   //
   ts << res.body;
   return str;
}

bool HttpServer::start(int port)
{
   return tcp_server.listen(QHostAddress::Any, port);
}

void HttpServer::onError(QAbstractSocket::SocketError socketError)
{
   qDebug() << "#tcp server error: " << socketError;
}

void HttpServer::onNewConnection()
{
  QObject * sender = this->sender();  // to get sender object pointer
  QTcpServer * server = static_cast<QTcpServer *>(sender);  // downcast
  //
  qDebug() << "#New connection: ";
  QTcpSocket * new_client = server->nextPendingConnection();
  // connect client socket signals
  connect(new_client, &QTcpSocket::readyRead,
      this, &HttpServer::onClientReadyRead);
  connect(new_client, &QTcpSocket::bytesWritten,
      this, &HttpServer::onClientDataSent);
  connect(new_client, &QTcpSocket::disconnected,
      this, &HttpServer::onClientDisconnected);
}

HttpResponse HttpServer::handleRequest(HttpRequest & req)
{
    HttpResponse res;
    res.http_version = "HTTP/1.1";
    HttpHandler handler = getHandler(req);
    if (handler == nullptr)
        handler = handler_;
    if (handler == nullptr)
    {
        res.setStatus(404, "Not Found");
    }
    else
    {
        req.query = parseUrlQuery(req.uri);
        res.setStatus(200, "OK");
        res.setContentType("text/html");
        handler(req, res);
    }
    res.setContentLength(res.body.length());
    return res;
}

void HttpServer::onClientReadyRead()
{
    QObject * sender = this->sender();
    QTcpSocket * client = static_cast<QTcpSocket *>(sender);
    //
    QString received_string = QString::fromUtf8(client->readAll());
    // check if there was a request header with content length
    if (current_request.contentLength() == 0) current_request = parseHttpRequest(received_string);
    else if(received_string.length() == current_request.contentLength()) current_request.body = received_string;
    else current_request.body += received_string;

    if(current_request.body.length() != 0)
        qDebug() << "[" <<current_request.body.length() << " of " << current_request.contentLength() << "]" << " saved";
    // check if the request is complete
    if (current_request.contentLength() == current_request.body.length())
    {
        HttpResponse res = handleRequest(current_request);
        client->write(formatHttpResponse(res).toUtf8());
        client->flush();
        current_request = HttpRequest{};  // clear request
    }
}

void HttpServer::onClientDataSent()
{
   //qDebug() << "data sent to client.";
   QObject * sender = this->sender();  // to get sender object pointer
   QTcpSocket * client = static_cast<QTcpSocket *>(sender);  // downcast
   // close the connection
   client->close();
}

void HttpServer::onClientDisconnected()
{
  QObject * sender = this->sender();  // to get sender object pointer
  QTcpSocket * client = static_cast<QTcpSocket *>(sender);  // downcast
  //
  qDebug() << "#client disconnected";
  client->deleteLater();  // use this instead of delete
}


HttpServer::HttpHandler HttpServer::getHandler(HttpRequest & req)
{
   QString path = req.uri.split("?")[0];
   HttpHandler handler = nullptr;
   QMap<QString, HttpHandler>::iterator it;
   for (it = handlers_.begin(); it != handlers_.end(); ++it)
   {
       if (patternMatch(it.key(), req.method + " " + path, req.params))
       {
           handler = it.value();
           break;
       }
   }
   return handler;
}


void HttpServer::setHandler(const HttpHandler & handler)
{
    handler_ = handler;
}
