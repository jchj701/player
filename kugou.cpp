#include "kugou.h"
#include "mainwindow.h"

#include <QEventLoop>
#include <QNetworkCookie>
QString KuGou::musicName1;

KuGou::KuGou(QObject *parent) : QObject(parent)
{
    network_manager = new QNetworkAccessManager();
    network_request = new QNetworkRequest();				//发送请求一得到AlbumID和FileHash
    network_manager2 = new QNetworkAccessManager();
    network_request2 = new QNetworkRequest();			//发送请求二得到url和歌词等信息

    network_request2->setHeader(QNetworkRequest::UserAgentHeader,
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.157 Safari/537.36");

    //	换成自己浏览器的Cookie
    //  Cookie: key1=value1;key2=value2;key3=value3

    QNetworkCookie c1 = QNetworkCookie("kg_mid", "b4c99d5854827943b35838ca8ec47036");
    QNetworkCookie c2 = QNetworkCookie("kg_dfid","4EANP823WmwR09XGUD1nf3lA");
    QNetworkCookie c3 = QNetworkCookie("kg_dfid_collect","d41d8cd98f00b204e9800998ecf8427e");
    QNetworkCookie c4 = QNetworkCookie("Hm_lvt_aedee6983d4cfc62f509129360d6bb3d","1558515861,1558524581,1558525842");
    QNetworkCookie c5 = QNetworkCookie("Hm_lpvt_aedee6983d4cfc62f509129360d6bb3d","1558525858");


    QList<QNetworkCookie> *cookies = new QList<QNetworkCookie>();
    cookies->push_back(c1);
    cookies->push_back(c2);
    cookies->push_back(c3);
    cookies->push_back(c4);
    cookies->push_back(c5);
    QVariant var;
    var.setValue(*cookies);
    network_request2->setHeader(QNetworkRequest::CookieHeader,var);

    connect(network_manager2, &QNetworkAccessManager::finished, this, &KuGou::replyFinished2);
    connect(network_manager, &QNetworkAccessManager::finished, this, &KuGou::replyFinished);


    qDebug() << "KuGou::KuGou(QObject *parent)";
}

void KuGou::replyFinished(QNetworkReply *reply)        //发送搜索请求完成，接受到信息，然后进行解析
{
    //获取响应的信息，状态码为200表示正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串

        parseJson_getAlbumID(result);  //自定义方法，解析歌曲数据
    }
    else
    {
        //处理错误
        qDebug()<<"处理错误";
    }
}
void KuGou::replyFinished2(QNetworkReply *reply)       //发送搜索请求完成，接受到信息，然后进行解析         通过歌曲ID搜索
{
    //获取响应的信息，状态码为200表示正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    qDebug() << "status_code" << status_code;


    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串

        parseJson_getplay_url(result);  //自定义方法，解析歌曲数据
    }
    else
    {
        //处理错误
        qDebug()<<"处理错误";
    }
}

void KuGou::parseJson_getAlbumID(QString json)     //解析接收到的歌曲信息，得到歌曲ID
{
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootObj = parse_doucment.object();
            if(rootObj.contains("data"))
            {
                QJsonValue valuedata = rootObj.value("data");
                if(valuedata.isObject())
                {
                    QJsonObject valuedataObject = valuedata.toObject();
                    if(valuedataObject.contains("lists"))
                    {
                        QJsonValue valueArray = valuedataObject.value("lists");
                        if (valueArray.isArray())
                        {
                            QJsonArray array = valueArray.toArray();
                            int size = array.size();
                            for(int i = 0;i < size;i++)
                            {
                                QJsonValue value = array.at(i);
                                if(value.isObject())
                                {
                                    QJsonObject object = value.toObject();
                                    if(object.contains("AlbumID"))
                                    {
                                        QJsonValue AlbumID_value = object.take("AlbumID");
                                        if(AlbumID_value.isString())
                                        {
                                            album_idStr = AlbumID_value.toString();             //歌曲ID信息
                                        }
                                    }
                                    if(object.contains("FileHash"))
                                    {
                                        QJsonValue FileHash_value = object.take("FileHash");
                                        if(FileHash_value.isString())
                                        {
                                            hashStr = FileHash_value.toString();                //hash
                                            //qDebug() << "FileHash = " << hashStr;
                                        }
                                    }

                                    //通过歌曲ID发送请求，得到歌曲url和歌词
                                    QString KGAPISTR1 = QString("http://www.kugou.com/yy/index.php?r=play/getdata&hash=%1&album_id=%2&_=1497972864535")
                                            .arg(hashStr).arg(album_idStr);

                                    //输入固定歌曲URL，测试用
                                    //KGAPISTR1 = QString("http://www.kugou.com/yy/index.php?r=play/getdata&hash=9DE51483789F431637D07BC2A6E6809A&album_id=&_=1497972864535");

                                    //显示当前获取歌曲URL
                                    //qDebug() << "parseJson_getAlbumID " << ",KGAPISTR1 = " << KGAPISTR1;

                                    network_request2->setUrl(QUrl(KGAPISTR1));
                                    network_manager2->get(*network_request2);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        qDebug()<<json_error.errorString();
    }
}

void KuGou::parseJson_getplay_url(QString json)        //解析得到歌曲
{
#if 0
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        QVariantMap result = parse_doucment.toVariant ().toMap ();
        QVariantMap dataList = result["data"].toMap ();
        qDebug() << "in parseJson_getplay_url";

        QString ss = dataList["play_url"].toString ();
        qDebug() << "ss = " << ss;
        foreach(QVariant data, dataList["play_url"].toList ())
        {
            QString url = data.toString ();
            qDebug() << "url = " << url;
        }
    }
#endif
#if 1
    //测试用json
    //json = "{\"status\":1,\"err_code\":0,\"data\":{\"hash\":\"3517475BA87746A792B5B3C0821CA1E4\",\"timelength\":260000,\"filesize\":4162133,\"audio_name\":\"\u8352\u6728\u6bec\u83dc - \u521d\u590f\u7269\u8bed\",\"have_album\":1,\"album_name\":\"5sing\u539f\u521b\u97f3\u4e50\u7cbe\u9009\",\"album_id\":\"10684687\",\"img\":\"http:\/\/singerimg.kugou.com\/uploadpic\/softhead\/400\/20150817\/20150817141102378988.jpg\",\"have_mv\":0,\"video_id\":0,\"author_name\":\"\u8352\u6728\u6bec\u83dc\",\"song_name\":\"\u521d\u590f\u7269\u8bed\",\"lyrics\":\"[00:00.45]\u521d\u590f\u7269\u8bed (\u300a\u4e03\u91cc\u9999\u300b\u65e5\u6587\u7248) - \u8352\u6728\u6bec\u83dc\r\n[00:03.47]\u8a5e\uff1a \u5c71\u5ddd\u5927\u6d77\r\n[00:04.64]\u66f2\uff1a\u5468\u6770\u4f26\r\n[00:18.98]\u96e8\u4e0a\u304c\u308a\r\n[00:21.93]\u5915\u7acb\u306e\u7a7a\u6c17\r\n[00:25.00]\u541b\u306e\u6a2a\u3067\r\n[00:28.12]\u5c0f\u3055\u306a\u4e0b\u99c4\u97f3\r\n[00:31.26]\u6570\u3048\u3066\u3044\u308b\r\n[00:34.34]\u7e4a\u7d30\u306a\u6b69\u5e45\u3068\r\n[00:37.28]\u8a00\u8449\u306b\u9837\u304f\u3060\u3051\u306e\u50d5\r\n[00:43.55]\u6b69\u304f\u8db3\u306f\r\n[00:46.32]\u524d\u3088\u308a\u9045\u3044\u306e\u306b\r\n[00:50.22]\u3044\u3064\u306e\u9593\u306b\u304b\r\n[00:52.38]\u307e\u305f\u96e2\u308c\u3066\u305f\r\n[00:55.86]\u6d74\u8863\u306e\u3072\u307e\u308f\u308a\r\n[00:58.62]\u8996\u754c\u304b\u3089\u6d88\u3048\u306a\u3044\u3088\u3046\u306b\r\n[01:02.29]\u305d\u3063\u3068\u6b69\u5e45\u3092\u5408\u308f\u305b\u3066\u308b\u3088\r\n[01:07.93]\u8a00\u8449\u306b\u3067\u304d\u306a\u3044\r\n[01:11.77]\u3053\u306e\u6c17\u6301\u3061\u306f\r\n[01:14.06]\u8fd1\u304f\u306b\u3044\u308b\u307b\u3069\u5bc2\u3057\u3044\u8a33\u306a\u306e\u304b\r\n[01:20.30]\u8749\u9054\u306e\u6b4c\u306b\u8074\u304d\u60da\u308c\u3066\u308b\u5185\r\n[01:27.34]\u50d5\u3060\u3051\u306e\u541b\u3067\u3044\u3066\r\n[01:33.01]\u8fd1\u3059\u304e\u308b\u3068\r\n[01:35.86]\u547c\u5438\u3082\u3067\u304d\u306a\u3044\r\n[01:39.01]\u7b11\u9854\u304c\u304f\u3063\u304d\u308a\u898b\u3048\u308b\r\n[01:43.17]\u8ddd\u96e2\u304c\u3061\u3087\u3046\u3069\u3044\u3044\r\n[01:45.24]\u9bc9\u9054\u306e\u6cf3\u304e\u306b\u898b\u60da\u308c\u3066\u308b\u5185\u306b\r\n[01:52.26]\u50d5\u3060\u3051\u306e\u541b\u3067\u3044\u3066\r\n[02:22.95]\u8a00\u8449\u306b\u3067\u304d\u306a\u3044\r\n[02:26.52]\u3053\u306e\u6c17\u6301\u3061\u306f\r\n[02:28.89]\u8fd1\u304f\u306b\u3044\u308b\u307b\u3069\u5bc2\u3057\u3044\u8a33\u306a\u306e\u304b\r\n[02:35.14]\u8749\u9054\u306e\u6b4c\u306b\u8074\u304d\u60da\u308c\u3066\u308b\u5185\r\n[02:42.14]\u50d5\u3060\u3051\u306e\u541b\u3067\u3044\u3066\r\n[02:48.29]\u3046\u3061\u308f\u7247\u624b\u306b\r\n[02:51.07]\u66c7\u3063\u305f\u30ac\u30e9\u30b9\u306b\r\n[02:54.87]\u300c\u697d\u3057\u3044\u306d\u300d\u3063\u3066\u5c0f\u3055\u304f\u66f8\u3044\u3066\u308b\r\n[03:00.49]\u7b11\u3044\u306a\u304c\u3089\u632f\u308a\u5411\u304f\u305d\u306e\u77ac\u9593\r\n[03:06.54]\u305f\u3060\u541b\u306e\u5507\u596a\u3044\u305f\u3044\r\n[03:12.78]\u8fd1\u3059\u304e\u308b\u3068\r\n[03:15.57]\u547c\u5438\u3082\u3067\u304d\u306a\u3044\r\n[03:18.75]\u7b11\u9854\u304c\u304f\u3063\u304d\u308a\u898b\u3048\u308b\r\n[03:22.97]\u8ddd\u96e2\u304c\u3061\u3087\u3046\u3069\u3044\u3044\r\n[03:24.98]\u9bc9\u9054\u306e\u6cf3\u304e\u306b\u898b\u60da\u308c\u3066\u308b\u5185\u306b\r\n[03:32.02]\u50d5\u3060\u3051\u306e\u541b\u3067\u3044\u3066\r\n[03:37.77]\u3053\u306e\u590f\u86cd\u304c\u611b\u3057\u3044\u3053\u3068\u306f\r\n[03:43.68]\u8fd1\u304f\u306b\u541b\u304c\u7b11\u3063\u3066\u308b\u304b\u3089\r\n[03:49.95]\u82b1\u706b\u304c\u6d88\u3048\u3066\u3044\u306a\u3044\u305d\u306e\u5185\u306b\r\n[03:56.96]\u50d5\u3060\u3051\u306e\u541b\u3067\u3044\u3066\r\n\",\"author_id\":\"191215\",\"privilege\":0,\"privilege2\":\"0\",\"play_url\":\"http:\/\/fs.w.kugou.com\/201905231423\/66b720ca89f6439281ffa40c2b779fdd\/G045\/M0A\/15\/18\/zYYBAFYKx9OARv6OAD-CVWisDeg240.mp3\",\"authors\":[{\"author_id\":\"191215\",\"sizable_avatar\":\"http:\/\/singerimg.kugou.com\/uploadpic\/softhead\/{size}\/20150817\/20150817141102378988.jpg\",\"is_publish\":\"1\",\"author_name\":\"\u8352\u6728\u6bec\u83dc\",\"avatar\":\"http:\/\/singerimg.kugou.com\/uploadpic\/softhead\/400\/20150817\/20150817141102378988.jpg\"}],\"bitrate\":128,\"audio_id\":\"18229341\"}}";

    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {

        if(parse_doucment.isObject())
        {
            QJsonObject rootObj = parse_doucment.object();
            if(rootObj.contains("data"))
            {
                qDebug() << "111";
                QJsonValue valuedata = rootObj.value("data");
                if(valuedata.isObject ())
                {
                    qDebug() << "222";
                    QJsonObject valuedataObject = valuedata.toObject();
                    QString play_urlStr("");
                    if(valuedataObject.contains("play_url"))
                    {  
                        QJsonValue play_url_value = valuedataObject.take("play_url");
                        if(play_url_value.isString())
                        {
                            play_urlStr = play_url_value.toString();                    //歌曲的url
                            qDebug() << "parseJson_getplay_url " << ",play_urlStr = " << play_urlStr;
                            if(play_urlStr!="")
                            {
                                emit mediaAdd(play_urlStr);
                            }
                        }
                    }
                    else
                    {
                        qDebug() << "版权歌曲，无url";
                    }

                    if(valuedataObject.contains("audio_name"))
                    {
                        QJsonValue play_name_value = valuedataObject.take("audio_name");
                        if(play_name_value.isString())
                        {
                            QString play_name = play_name_value.toString();                //歌曲名字
                            if(play_urlStr!="")
                            {
                                KuGou::musicName1 = play_name;
                                qDebug() << "KuGou::musicName1 = " << KuGou::musicName1;
                                emit nameAdd(play_name);
                            }

                        }
                    }
                    if(valuedataObject.contains("lyrics"))                                  //lrc
                    {
                        QJsonValue play_url_value = valuedataObject.take("lyrics");
                        if(play_url_value.isString())
                        {
                            QString play_lrcStr = play_url_value.toString();
                            if(play_urlStr!="")
                            {
                                if(play_lrcStr != "")
                                {
                                    emit lrcAdd(play_lrcStr);
                                }
                                else
                                {
                                    emit lrcAdd("没有歌词");
                                }
                            }

                        }
                    }
                }
            }
        }
    }
#endif
}
void KuGou::search(QString str)
{
    //发送歌曲搜索请求
    QString KGAPISTR1 = QString("http://songsearch.kugou.com/song_search_v2?keyword=%1&page=&pagesize=&userid=-1&clientver=&platform=WebFilter&tag=em&filter=2&iscorrection=1&privilege_filter=0").arg(str);

    qDebug() << "in search" << "KGAPISTR1:" << KGAPISTR1;
    network_request->setUrl(QUrl(KGAPISTR1));
    network_manager->get(*network_request);
}


