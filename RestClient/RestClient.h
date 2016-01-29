#ifndef RESTCLIENT_H
#define RESTCLIENT_H

#include <QWidget>
#include <QObject>
#include <QNetworkRequest>
#include <QByteArray>
#include <string>
#include <map>

class QNetworkAccessManager;
class QNetworkReply;

class RestClient : public QObject
{
    Q_OBJECT
public:
    RestClient(QWidget *parent = 0);
    ~RestClient();

    bool login(QString name, QString password);
    bool version(QString app, QString build);
    std::string decodeData(const char* key);
    bool requestFinished(QNetworkReply *);

private:
//members
    typedef std::map<QString, QString> Map;
    QString mRequestType;
    Map mResults;
    QNetworkAccessManager *mManager;
};

#endif // RESTCLIENT_H
