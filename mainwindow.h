#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>
#include <QtCore>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    Mat frame;
    VideoCapture cap;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void Frame();
    void readSerial();
    void IdWebcam();

    const string date();
    const string heure();

    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();
    void on_radioButton_4_clicked();
    void on_radioButton_5_clicked();

private:
    Ui::MainWindow *ui;

    QSerialPort *arduino;
    static const quint16 arduino_uno_vendor_id = 6790; //nano
    static const quint16 arduino_uno_product_id = 29987; //nano

    QByteArray serialData;
    QString serialBuffer;

    QString parseData1;
    QString parseData2;

    float angle;
    float heading;

    int x0 = 320;
    int y0 = 240;
    int x1 = 160;
    int x2 = 480;

    Point center = Point(x0, y0);

    float rad = (3.1415927/180);

    Scalar Blanc = Scalar(255,255,255);
    Scalar Vert = Scalar(0,255,0);
    Scalar Rouge = Scalar(0,0,255);
    Scalar Bleu = Scalar(255,0,0);
    Scalar Noir = Scalar(0,0,0);
    Scalar couleur;

    int x = 0;
    int y;
};

#endif // MAINWINDOW_H
