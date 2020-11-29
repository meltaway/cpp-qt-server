#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QMap>

struct HttpRequest
{
   QString method;
   QString uri;
   QString http_version;
   QMap<QString, QString> headers;
   QString body;
   //
   QMap<QString, QString> params;
   QMap<QString, QString> query;
   //
   QString contentType() const;
   int contentLength() const;
};

struct HttpResponse
{
   QString http_version;
   int status_code;
   QString status_description;
   QMap<QString, QString> headers;
   QString body;
   //
   void setContentType(const QString & type);
   void setContentLength(int length);
   void setStatus(int code, const QString & description = "");
};



class HttpServer : public QObject
{
  QTcpServer tcp_server;
  HttpResponse handleRequest(HttpRequest & req);
  using HttpHandler = std::function<void (HttpRequest & req, HttpResponse & res)>;
  HttpHandler handler_ = nullptr;
  HttpRequest current_request;
  HttpHandler getHandler(HttpRequest & req);

  QMap<QString, HttpHandler> handlers_;

  Q_OBJECT
public:
  explicit HttpServer(QObject * parent = nullptr);

  bool start(int port);

  void setHandler(const HttpHandler & handler);

  void get(const QString & uri, const HttpHandler & handler) { handlers_["GET " + uri] = handler; }

  void post(const QString & uri, const HttpHandler & handler) { handlers_["POST " + uri] = handler; }

  void put(const QString & uri, const HttpHandler & handler) { handlers_["PUT " + uri] = handler; }

  void delete_(const QString & uri, const HttpHandler & handler) { handlers_["DELETE " + uri] = handler; }

private slots:
  void onError(QAbstractSocket::SocketError socketError);
  void onNewConnection();

  void onClientReadyRead();
  void onClientDataSent();
  void onClientDisconnected();

};



#endif // MYTCPSERVER_H
