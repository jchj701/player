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

int MainWindow::model = 1;
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
//    list = new QListWidget(this);
//    message = new QLabel(this);
//    playerList = new QMediaPlaylist();
//    player = new QMediaPlayer();
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
        //list->addItem (musicName);
        //message->setText (tr("添加成功"));
        qDebug() << "加入成功:" << fileNameList.at (i);

        ui->listWidget->addItem (musicName);
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
        ui->pushButton_5->setText (tr("顺序播放"));

    }
    else if(model == 1)
    {
        //随机播放
        playerList->setPlaybackMode(QMediaPlaylist::Random);
        ui->pushButton_5->setText (tr("随机播放"));
    }

    else
    {
        //单曲循环
        model = 2;
        playerList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->pushButton_5->setText (tr("列表循环"));
    }
    qDebug() << "now" << model;
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
    playerList->next();
}

void MainWindow::volumChange(int vol)
{
    //关联滑块的移动，将值设置播放的音量
    player -> setVolume(vol);
}

void MainWindow::positionChange(qint64 position)
{
    qDebug() << "in positionChange";
    ui->processHorizontalSlider->setMaximum(player->duration() / 1000);//设置滑块的长度范围为音乐长.00度
    ui->processHorizontalSlider->setValue(position / 1000);//如果音乐进度改变，则改变滑块显示位置
    moved = position;
    qDebug() << "position = " << position;
    qDebug() << "moved = " << moved;
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

void MainWindow::on_pushButton_4_clicked()
{
    addMoremusic ();
}

void MainWindow::on_pushButton_2_clicked()
{
    playMusic();

    //positionChange(100000);
    //ui->textBrowser->setText (playerList->currentMedia ());
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
        qDebug() << i;
        ui->pushButton_6->setText (i);
    }
    volumChange(value);

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
    qDebug() << "sliderMoved";
    qDebug() << position;
    seekChange(position);
}
