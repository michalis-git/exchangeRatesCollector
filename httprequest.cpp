#include "httprequest.h"
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qsslconfiguration.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QStringList>
#include <QJsonValue>
#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QStandardPaths>

#define RATES_DIR_NAME "exchange_rates"

HttpRequest::HttpRequest(QString urlString, QCoreApplication *app) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestReceived(QNetworkReply*)));

    mApp = app;

    QString rootPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString ratesPath = rootPath + QDir::separator() + RATES_DIR_NAME;
    if (!QDir(ratesPath).exists())
        QDir(rootPath).mkdir(RATES_DIR_NAME);
    m_ratespath = ratesPath;

    QStringList nameFilter;
    nameFilter << "*.txt";
    QFileInfoList fileInfoList = QDir(m_ratespath).entryInfoList(nameFilter,
                                                                 QDir::Dirs | QDir::Files
                                                                 | QDir::NoSymLinks | QDir::NoDot
                                                                 | QDir::NoDotDot);
    for (auto &fileInfo : fileInfoList) {
        m_filePathMap.insert(fileInfo.baseName().toUpper(), fileInfo.absoluteFilePath());
    }

    QUrl url(urlString);
    QNetworkRequest request(url);
    request.setRawHeader( "User-Agent" , "Mozilla Firefox" );
    manager->get(request);
}

HttpRequest::~HttpRequest() {
}

void HttpRequest::readJsonObject(const QString & date, const QJsonObject &json) {
    QString dateString = date.left(10);
    foreach(const QString& key, json.keys()) {
        QJsonValue value = json.value(key);

        double rate = value.toDouble();
        QString rateString = QString::number(rate);

        if (m_filePathMap.contains(key)) {
            QFile exchangeRateFile(m_filePathMap.value(key));
            if (exchangeRateFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream outStream(&exchangeRateFile);
                outStream << dateString + " " + rateString + "\n";
                exchangeRateFile.close();
            }
        } else {
            QString fileFullPath = m_ratespath + QDir::separator() + key.toLower() + ".txt";
            QFile exchangeRateFile(fileFullPath);
            if (exchangeRateFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream outStream(&exchangeRateFile);
                outStream << dateString + " " + rateString + "\n";
                exchangeRateFile.close();
            }
        }
    }
}

void HttpRequest::requestReceived(QNetworkReply *reply) {
    reply->deleteLater();
    qDebug() << reply->error();
    if(reply->error() == QNetworkReply::NoError) {
        // Get the http status code
        int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << v;
        if (v >= 200 && v < 300) { // Success

            QByteArray result = reply->readAll();
            QJsonDocument itemDoc = QJsonDocument::fromJson(result);
            QJsonObject rootObject = itemDoc.object();

            // get the response object
            QJsonValue response = rootObject.value("response");
            QJsonObject responseObj = response.toObject();

            QString date;
            QJsonValue dateValue = responseObj.value("date");
            if (dateValue.isString()) {
                date = dateValue.toString();
            }
            // get the object of rates
            QJsonValue itemArrayValue = responseObj.value("rates");
            if (itemArrayValue.isObject()) {
                qDebug() << "is object";
                QJsonObject ratesObject = itemArrayValue.toObject();
                qDebug() << ratesObject.value("AUD");
                readJsonObject(date, ratesObject);
            }
        }
        else if (v >= 300 && v < 400) { // Redirection
            // Get the redirection url
            QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            // Because the redirection url can be relative,
            // we have to use the previous one to resolve it
            newUrl = reply->url().resolved(newUrl);

            return; // to keep the manager for the next request
        }
    } else {
        qDebug() << "Error: Failed to update exchange rate files. Please check your internet connection and run the application again, manually.";
    }

    reply->manager()->deleteLater();

    emit requestIsFinished();
    mApp->quit();
}

