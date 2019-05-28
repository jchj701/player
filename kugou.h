#ifndef KUGOU_H
#define KUGOU_H

#include <QObject>
#include<QMediaPlaylist>
#include <QMediaPlayer>
#include<QPaintEvent>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QJsonValue>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>

class KuGou : public QObject
{
    Q_OBJECT
public:
    explicit KuGou(QObject *parent = nullptr);
    void parseJson_getAlbumID(QString json);
    void parseJson_getplay_url(QString json);

    void search(QString str);
    static QString musicName1;

signals:
    void nameAdd(QString name);
    void mediaAdd(QString urlStr);
    void lrcAdd(QString lrcStr);
    void metaDataAvailableChanged_1(bool available);
public slots:
    void replyFinished(QNetworkReply *reply);
    void replyFinished2(QNetworkReply *reply);
private:
    QNetworkAccessManager* network_manager;
    QNetworkRequest* network_request;
    QNetworkAccessManager* network_manager2;
    QNetworkRequest* network_request2;

    QString hashStr;
    QString album_idStr;
//    void listAdd(QString s);


};

#endif // KUGOU_H
