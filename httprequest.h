#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "qobject.h"
#include <qmap.h>
#include <QStringList>
#include <QMap>
#include <QCoreApplication>

class QNetworkReply;
class QScriptValue;
//class QStringList;
class MainWindow;

class HttpRequest : public QObject
{
    Q_OBJECT

public:
    HttpRequest(QString url, QCoreApplication *app);
    ~HttpRequest();
    QMap<QString, QString> designsAuthors;
    QMap<QString, float> designsWingsPercentageMap;
    QString authorName, designName;
    QStringList* authorSingularList;
    QStringList mlist;
    QString currencyName;
    QString dirPath;
    QString urlOfRequest;
    QStringList fullPathList;
    QMap<QString, QString> m_filePathMap;
    QCoreApplication *mApp;
    QString m_ratespath;

private:
    MainWindow* window;
    void parseJson(QByteArray result);
    void readJsonObject(const QString & date, const QJsonObject &json);

private slots:
    void requestReceived(QNetworkReply *reply);

signals:
    void requestIsFinished();
    void rateRequestIsFinished(QString currencyName);
};

#endif // HTTPREQUEST_H
