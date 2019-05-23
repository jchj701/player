#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QFileInfo>
#include "kugou.h"

int MainWindow::model = 2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer1 = new QTimer(this);
    timer2 = new QTimer(this);
    add = false;
    moved = 0;
    player = new QMediaPlayer(this);
    playerList = new QMediaPlaylist(this);

    volumChange(30);
    connect(player, SIGNAL(metaDataAvailableChanged(bool)), this, SLOT(showMessage(bool)));
    connect(ui->processHorizontalSlider,SIGNAL(sliderMoved(int)), this,SLOT(seekChange(int)));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChange(qint64)));
    qDebug() << "Music 构造函数";


    QFile file(":/qss/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = tr(file.readAll());
    this->setStyleSheet(styleSheet);
    file.close();

}

MainWindow::~MainWindow()
{
    delete player;
    delete playerList;
    delete ui;
}


void MainWindow::addMoremusic()
{
    QString musicFilePath, musicName;
    QFileInfo info;

    QStringList fileNameList = QFileDialog::getOpenFileNames (this, tr("打开文件"), "", tr("music(*.mp3)"));
    if(!fileNameList.isEmpty ())
    {
        add = true;
    }

    //本地歌曲逐一加入playerList
    for(int i = 0; i < fileNameList.size (); i++)
    {
        playerList->addMedia (QUrl::fromLocalFile (fileNameList.at(i)));

        //获取歌曲信息
        musicFilePath = fileNameList.at(i);
        info = QFileInfo(musicFilePath);
        musicName = info.fileName ();

        qDebug() << "加入成功:" << fileNameList.at (i);

        ui->listWidget->addItem (musicName);
    }

    playerList->setCurrentIndex (0);

    musicPlayPattern();
    player -> setPlaylist(playerList);

}

void MainWindow::addMoremusicForNet()
{
    QString musicFilePath, musicName;
    add = true;

    if(KuGou::musicName1 != "")
    {
        qDebug() << "加入成功:" << KuGou::musicName1;

        ui->listWidget->addItem (KuGou::musicName1);
    }

    playerList->setCurrentIndex (0);

    musicPlayPattern();
    player -> setPlaylist(playerList);

}

void MainWindow::setPlayModel()
{
    if(model == 0)
    {
        playerList->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(model == 1)
    {
        playerList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else
    {
        playerList->setPlaybackMode(QMediaPlaylist::Sequential);
    }
}

void MainWindow::playMusic()
{
    if(!add)
    {
        QMessageBox::information(this,"添加音乐","请先添加音乐","确定");
    }
    else
    {
        if(player -> state() == QMediaPlayer::PlayingState)//判断是否为播放模式
        {
            //设置播放模式为暂停
            player -> pause();
            timer1 ->stop();
        }
        else
        {
            //否则设置播放模式
            player -> play();
            connect(timer1, SIGNAL(timeout()), this, SLOT(posChange()));
            timer1->start(1000);
        }
    }


}

void MainWindow::posChange()
{

    if(moved >= player -> duration())
    {
        qDebug() << "重新点击播放";
        moved = 0;
    }
}

void MainWindow::musicPlayPattern()
{

    //z=++z%3;
    if(model == 0)
    {
        //顺序播放
        playerList->setPlaybackMode(QMediaPlaylist::Sequential);
        //ui->pushButton_5->setText (tr("顺序播放"));
        ui->pushButton_5->setStyleSheet (QString("QPushButton#pushButton_5{border-image: url(:/shunxu.png);}"));
    }
    else if(model == 1)
    {
        //随机播放
        playerList->setPlaybackMode(QMediaPlaylist::Random);
        //ui->pushButton_5->setText (tr("随机播放"));
        ui->pushButton_5->setStyleSheet (QString("QPushButton#pushButton_5{border-image: url(:/suiji.png);}"));
    }

    else
    {
        //单曲循环
        model = 2;
        playerList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        //ui->pushButton_5->setText (tr("列表循环"));
        ui->pushButton_5->setStyleSheet (QString("QPushButton#pushButton_5{border-image: url(:/Zombi Mushroom.ico);}"));
    }
    qDebug() << "now play model = " << model;
}

void MainWindow::preMusic()
{
    //上一首，并更新播放时间为0
    moved = 0;
    playerList ->previous();

}

void MainWindow::nextMusic()
{
    //下一首，更新播放时间为0
    moved = 0;
    playerList->next ();
    //playerList->setCurrentIndex (playerList->nextIndex ());

}

void MainWindow::volumChange(int vol)
{
    //关联滑块的移动，将值设置播放的音量
    player -> setVolume(vol);
}

void MainWindow::positionChange(qint64 position)
{
    //qDebug() << "in positionChange";
    ui->processHorizontalSlider->setMaximum(player->duration() / 1000);//设置滑块的长度范围为音乐长.00度
    ui->processHorizontalSlider->setValue(position / 1000);//如果音乐进度改变，则改变滑块显示位置
    moved = position;
    //qDebug() << "position = " << position;
    //qDebug() << "moved = " << moved;
    QTime moveTime(0,(moved/60000) % 60,(moved / 1000) % 60);
    ui->label_3->setText(moveTime.toString("mm:ss"));
}

void MainWindow::seekChange(int position)
{
    player->setPosition(position * 1000);//如果滑块改变位置，则改变音乐进度
    moved = position * 1000;//让播放时间等于音乐进度（毫秒）
    QTime moveTime(0,(moved/60000) % 60,(moved / 1000) % 60);//设置时间
    ui->label_3->setText(moveTime.toString("mm:ss"));//显示时间
}

void MainWindow::showMessage(bool ok)
{
    if(ok)
    {
        qDebug() << "showMessage";
        QString name= player->metaData(QMediaMetaData::Title).toString();
        QString author= player->metaData(QMediaMetaData::Author).toString();
        ui->textBrowser->setText("正在播放："+name + " -" + author);

        QTime  displayTime(0,(player -> duration() / 60000) % 60,
                           (player ->duration() / 1000) % 60);
        ui->label_3->setText(displayTime.toString("mm:ss"));
    }
}

void MainWindow::listAdd(QString s)
{
    qDebug() << "void MainWindow::listAdd(QString s)";

    ui->listWidget->addItem (s);
    //showMessage(true);
    emit SIGNAL(metaDataAvailableChanged(true));
}
void MainWindow::lrcStrAdd(QString s)
{
    lrcList.append(s);
}
void MainWindow::urlListAdd(QString s)
{
    qDebug() << "addMedia = " << s;
    playerList->addMedia(QUrl(s));
}

void MainWindow::on_pushButton_4_clicked()
{
    addMoremusic ();
}

void MainWindow::on_pushButton_2_clicked()
{
    playMusic();
    playOrPauseButtonChange();
}

void MainWindow::playOrPauseButtonChange()
{
    if(player->state () == QMediaPlayer::PlayingState)
    {
        qDebug() << "playing";
        ui->pushButton_2->setStyleSheet (QString("QPushButton#pushButton_2:hover{border-image:url(:/play1.png);} \
                                              QPushButton#pushButton_2:!hover{border-image:url(:/play2.png);}"));

    }
    else {
        qDebug() << "not playing, in other state;";
//        ui->pushButton_2->setProperty ("isPlay", "true");
        ui->pushButton_2->setStyleSheet (QString("QPushButton#pushButton_2:!hover{border-image:url(:/play4.png);}\
                                              QPushButton#pushButton_2:hover{border-image:url(:/play5.png);}"));
    }
}

void MainWindow::on_pushButton_clicked()
{
    preMusic ();
}

void MainWindow::on_pushButton_3_clicked()
{
    nextMusic ();
}

void MainWindow::on_pushButton_5_clicked()
{
    model--;
    musicPlayPattern();
}

void MainWindow::on_verticalSlider_valueChanged(int value)
{
    if(!value)
    {
        ui->pushButton_6->setText (tr("静音"));
    }
    else {
        QString i = QString::number (ui->verticalSlider->value ());
        qDebug() << "volume = " <<i;
        ui->pushButton_6->setText (i);
    }
    volumChange(value);

    ui->dial->setValue (value);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(ui->pushButton_6->text () == "静音")
    {
        ui->pushButton_6->setText (tr("未静音"));
        ui->verticalSlider->setValue (CurrentValue);
        player->setMuted (false);
    }
    else {
        CurrentValue = ui->verticalSlider->value ();
        ui->pushButton_6->setText (tr("静音"));
        ui->verticalSlider->setValue (0);
        player->setMuted (true);
    }
}

void MainWindow::on_processHorizontalSlider_sliderMoved(int position)
{
    qDebug() << "sliderMoved = " << position;
    seekChange(position);
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    //playerList->setCurrentIndex (currentRow);
    qDebug() << "currentRow = " << currentRow;
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    int row = ui->listWidget->currentRow ();
    playerList->setCurrentIndex (row);
    player->play ();
    playOrPauseButtonChange();
}


void MainWindow::on_pushButton_7_clicked()
{
    KuGouSearch = new KuGou(this);

    connect(KuGouSearch,SIGNAL(nameAdd(QString)),this,SLOT(listAdd(QString)));
    connect(KuGouSearch,SIGNAL(lrcAdd(QString)),this,SLOT(lrcStrAdd(QString)));
    connect(KuGouSearch,SIGNAL(mediaAdd(QString)),this,SLOT(urlListAdd(QString)));

    if(player->state () == QMediaPlayer::PlayingState)
    {
        on_pushButton_2_clicked ();
    }

    player->setPlaylist (nullptr);
    lrcList.clear();
    lrcStr.clear();
    lrcTime.clear();
    playerList->clear();
    ui->listWidget->clear();


    KuGouSearch->search (ui->lineEdit->text ());

    player->setPlaylist (playerList);
    add =true;

    //addMoremusicForNet();
    if(playerList->currentIndex () >= 0)
    {
        qDebug() << "playerList->currentIndex () >= 0";
        //get_lrcStrTime();
    }
}

