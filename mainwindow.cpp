#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <string>
#include <QDebug>
#include <QMessageBox>

#define FILE "/home/tux/test"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serialBuffer = "";

    parseData1 = "";
    parseData2 = "";

    arduino = new QSerialPort(this);

    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Description: " << serialPortInfo.description() << "\n";
        qDebug() << "Has vendor id?: " << serialPortInfo.hasVendorIdentifier() << "\n";
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier() << "\n";
        qDebug() << "Has product id?: " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier() << "\n";
    }

    bool arduino_is_available = false;
    QString arduino_uno_port_name;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if((serialPortInfo.productIdentifier() == arduino_uno_product_id)
                    && (serialPortInfo.vendorIdentifier() == arduino_uno_vendor_id))
            {
                arduino_is_available = true;
                arduino_uno_port_name = serialPortInfo.portName();
            }
        }
    }

    if(arduino_is_available)
    {
        qDebug() << "Found the arduino port...\n";
        arduino->setPortName(arduino_uno_port_name);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud9600);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        QObject::connect(arduino, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }
    else
    {
        qDebug() << "Couldn't find the correct port for the arduino.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to arduino.");
    }
}

MainWindow::~MainWindow()
{
    if(arduino->isOpen())
    {
        arduino->close();
    }

    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    cap.open(CAP_ANY);

    while(true)
    {
        cap >> frame;

        circle(frame, center, 2, couleur,2);

        heading = parseData1.toFloat();
        string a = parseData1.toStdString().c_str();

        angle = parseData2.toFloat();
        string b = parseData2.toStdString().c_str();

        rectangle(frame, Point(290,3), Point(375, 40), Scalar(0,0,0),-1);
        putText(frame, "Heading", Point(300, 15), 1, 1, Scalar(255,255,255),1);
        putText(frame, a, Point(300,30), 1, 1, Scalar(255,255,255),1);

        rectangle(frame, Point(290,165), Point(350, 210), Scalar(0,0,0),-1);
        putText(frame, "Alpha", Point(300, 180), 1, 1, Scalar(255,255,255),1);
        putText(frame, b, Point(300,200), 1, 1, Scalar(255,255,255),1);


        float x3 = ((x1 - x0) * cos(angle *rad)) + x0;
        float y3 = ((x1 - x0) * sin(angle*rad)) + y0;

        float x4 = ((x2 - x0) * cos(angle *rad)) + x0;
        float y4 = ((x2 - x0) * sin(angle*rad)) + y0;

        line(frame, center, Point(x3, y3), couleur,1);
        line(frame, center, Point(x4, y4), couleur,1);

        rectangle(frame, Point(3,4), Point(110,42), Scalar(0,0,0),-1);;
        putText(frame, date(),Point(3,20),1,1,Blanc,1);
        putText(frame, heure(),Point(3,40),1,1,Blanc,1);

        line(frame, Point(110,240), Point(140, 240), couleur,2);
        line(frame, Point(530,240), Point(500, 240), couleur,2);

        cout << date() << " " << heure() << " " << "Heading: " << heading << " " << "Angle: " << angle << endl;
        freopen(FILE, "a", stdout);

        IdWebcam();
        Frame();

        waitKey(32);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    destroyAllWindows();
    close();
    exit(0);
}

void MainWindow::Frame()
{
    cv::resize(frame, frame, Size(ui->label->width(), ui->label->height()));
    cvtColor(frame, frame,CV_BGR2RGB);

    QImage imgFrame= QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap pixFrame = QPixmap::fromImage(imgFrame);

    ui->label->setPixmap(pixFrame);
}

void MainWindow::readSerial()
{
    QStringList buffer_split = serialBuffer.split(",");

    if(buffer_split.length() < 4) //3 datas
    {
        serialData = arduino->readAll();
        serialBuffer += QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }
    else
    {
        serialBuffer = "";
        //qDebug() << buffer_split << endl;

        parseData2 = buffer_split[1];
        parseData1 = buffer_split[0];

        qDebug() << parseData1 << " " << parseData2 << endl;

       // float a = parseData1.toFloat();
       // float b = parseData2.toFloat();
        //qDebug() << a << endl;
    }
}

void MainWindow::IdWebcam()
{
    int y = cap.get(x);
    QString s = QString::number(y);
    ui->labelCam->setText(s);
}

const string MainWindow::date()
{
    time_t     now = time(0);
    struct tm  tstruct;
     char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%d-%m-%Y", &tstruct);

    return buf;
}

const string MainWindow::heure()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%X", &tstruct);

    return buf;
}

void MainWindow::on_radioButton_clicked()
{
    couleur = Blanc;
}

void MainWindow::on_radioButton_2_clicked()
{
    couleur = Noir;
}

void MainWindow::on_radioButton_3_clicked()
{
    couleur = Rouge;
}

void MainWindow::on_radioButton_4_clicked()
{
    couleur = Vert;
}

void MainWindow::on_radioButton_5_clicked()
{
    couleur = Bleu;
}

void MainWindow::on_pushButton_3_clicked()
{
    y = system("/usr/bin/xed /home/tux/test");
}
