#include "RestClient.h"

#include <QDebug>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QEventLoop>
#include <QByteArray>
#include <string>
#include <map>


static const char* REQUEST_URL = "http://localhost/";



RestClient::RestClient(QWidget *parent)
    : QObject(parent)
{
    mManager = new QNetworkAccessManager(this);
    login("azaza", "lalka");
}

RestClient::~RestClient()
{
}



bool RestClient::login(QString name, QString password)
{
    QString loginUrl = REQUEST_URL;
    loginUrl += "login";
    mRequestType = "login";

    QNetworkRequest request;
    request.setUrl(QUrl(loginUrl));
    request.setRawHeader("Authorization", "Basic " +
                         QByteArray(QString("%1:%2").arg(name).arg(password).toUtf8()).toBase64());

    QNetworkReply *reply = mManager->get(request);
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    requestFinished(reply);
    qDebug() << decodeData("success").c_str();
    return true;
}


bool RestClient::version(QString app, QString build)
{
    QString versionUrl = REQUEST_URL;
    versionUrl += "version?app=" + app + "&build=" + build;
    mRequestType = "version";

    QNetworkRequest request;
    request.setUrl(QUrl(versionUrl));

    QNetworkReply *reply = mManager->get(request);
    QEventLoop loop;
    connect(mManager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));
    loop.exec();

    requestFinished(reply);
    decodeData("build");

    return true;
}

bool RestClient::requestFinished(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();
    QString value(data);
    mResults[mRequestType] = value;
    return true;
}

std::string RestClient::decodeData(const char* key)
{
    Map::iterator dataToParseKey = mResults.find(mRequestType);
    QJsonDocument parsedDataDocument = QJsonDocument::fromJson(dataToParseKey->second.toStdString().c_str());
    QJsonObject parsedData = parsedDataDocument.object();
    QJsonObject::iterator returnValue = parsedData.find(QString(key));

    return returnValue.value().toString().toStdString();
}
