#include <QCoreApplication>
#include <QApplication>
#include <QString>
#include <QDebug>
#include "httprequest.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QString api_key = "48de26090d21686a8db30b43dfe6a3e3";
    QString endpoint = "latest?";
    QString base = "base=EUR&";
    QString url = "https://api.currencyscoop.com/v1/";
    url.append(endpoint);
    url.append(base);
    url.append("api_key=" + api_key);

    HttpRequest request(url, &a);

    return a.exec();
}
