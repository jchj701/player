#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTime>
#include <QPaintEvent>
#include <QMediaPlaylist>
#include <QTimer>
#include <QListWidget>
#include "kugou.h"

namespace Ui {
    class MainWindow;
    }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static int model;

private slots:
    void on_pushButton_4_clicked();
    void addMoremusic();
    void playMusic();
    void preMusic();
    void nextMusic();
    void setPlayModel();
//    void meteOpen();
    void volumChange(int);
    void positionChange(qint64 position);
    void showMessage(bool);
    void seekChange(int position);
    void posChange();
//    void clearMessage();
    void musicPlayPattern();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_pushButton_6_clicked();

    void on_processHorizontalSlider_sliderMoved(int position);

    void on_listWidget_currentRowChanged(int currentRow);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_7_clicked();

    void listAdd(QString s);
    void lrcStrAdd(QString s);
    void urlListAdd(QString s);
    void addMoremusicForNet();

    //void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::MainWindow *ui;
//    void init_controls();
//    void init_skin();

//    QPushButton *BtnClose;
//    QPushButton *BtnMin;
//    QPushButton *BtnPlay;
//    QPushButton *BtnPrev;
//    QPushButton *BtnNext;
//    QPushButton *muteButton;
//    QPushButton *addMore;
//    QPushButton *playPattern;
//    QSlider     *volumeControl;
//    QSlider     *seekSlider;
//    QLabel      *showTime;
//    QLabel      *showPro;
//    QLabel      *showMge;
//    QLabel      *title;
    QLabel      *message;
    QListWidget *list;
    bool        add;
    QTimer      *timer1;
    QTimer      *timer2;
    int         moved;
//    QPoint      dragPosition;
    QMediaPlayer   *player;
    QMediaPlaylist * playerList;
    int CurrentValue;

    KuGou *KuGouSearch;
    QStringList lrcTime;
    QStringList lrcStr;
    QStringList lrcList;
    void playOrPauseButtonChange();
protected:
//    void paintEvent(QPaintEvent *event);
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
};


#endif // MAINWINDOW_H
