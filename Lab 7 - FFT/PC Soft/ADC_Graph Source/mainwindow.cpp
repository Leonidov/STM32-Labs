#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtSerialPort/QtSerialPort"
#include "QList"
#include "QTimer"

//таймаут ком-потра 1 секунда
#define COM_TIMEOUT 500

//частота дискретизации, кГц
#define Fs 100000

//кол-во отсчетов
#define SAMPLES 256
#define V_REF   3.3

QSerialPort *serial;
QString ReadBuffer;
bool AnsReceived;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort;

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    QList<QSerialPortInfo> portList;
    portList = QSerialPortInfo::availablePorts();

    for (int i=0; i<portList.size(); i++)
    {
        QString pname=portList[i].portName();

        #ifdef Q_OS_MAC
        pname.prepend("cu.");
        #endif

        ui->PortsBox->addItem(pname);
    }

    ui->statusBar->showMessage("Ни к чему не подключено");

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(Timer_Ovf()));

    /***************************************************************/
    // создаем график и добавляем данные:
    ui->scope->addGraph();

    // задаем имена осей координат
    ui->scope->xAxis->setLabel("Время, мкс");
    ui->scope->yAxis->setLabel("Амплитуда, В");
    // задаем размеры осей
    ui->scope->xAxis->setRange(0, SAMPLES*10);
    ui->scope->yAxis->setRange(0, V_REF);

    ui->scope->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    connect(ui->scope, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
    /***************************************************************/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Menu_Close_triggered()
{
    close();
}

void MainWindow::on_action_triggered()
{
    QMessageBox::about(this,"О программе", "<h3>Мегаосциллограф</h3>"
                                           "<p>на базе МК STM32F100</p>"
                                           "<hr>"
                                           "<p>(с) Vladimir Leonidov</p>");
}

void MainWindow::on_ConnectBtn_clicked()
{
    if (ui->ConnectBtn->text()=="Подключиться")
    {
        serial->setPortName(ui->PortsBox->currentText());
        serial->setBaudRate(ui->BaudRateBox->currentText().toInt());
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);

        if (serial->open(QIODevice::ReadWrite))
        {
            ui->statusBar->showMessage("Подключён, можно работать!");
            ui->ConnectBtn->setText("Отключиться");
            ui->PortsBox->setEnabled(false);
            ui->BaudRateBox->setEnabled(false);
            ui->StartBtn->setEnabled(true);
        }
        else
        {
            QMessageBox::information(this,"Недоосциллограф","Что-то пошло не так");
        }
    }
    else
    {
        serial->close();
        ui->ConnectBtn->setText("Подключиться");
        ui->statusBar->showMessage("Ни к чему не подключено");
        ui->PortsBox->setEnabled(true);
        ui->BaudRateBox->setEnabled(true);
        ui->StartBtn->setEnabled(false);
    }
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    ReadBuffer.append(data);
    if (ReadBuffer.indexOf(0x0D,0)>0)
    {
       ReadBuffer.remove(0x0D);
       AnsReceived=true;
    }
}

void MainWindow::SendData(QString data)
{
    QByteArray str;
    str.append(data);
    str.append(0x0D);
    serial->write(str);
}

QString MainWindow::DoCommand(QString cmd)
{
    QTimer TO_Timer;
    QString Reply;

    TO_Timer.setSingleShot(true);
    AnsReceived=false;
    SendData(cmd);
    TO_Timer.start(COM_TIMEOUT);
    while (!AnsReceived & TO_Timer.isActive())
        QApplication::processEvents();
    if (!TO_Timer.isActive())
        Reply="TimeOut";
    else
        Reply=ReadBuffer;
    ReadBuffer.clear();
    return Reply;
}

float get_max(QVector<double> mass)
{
    float max=0;
    for (int i=0;i<mass.length();i++)
        if (mass[i] > max) max=mass[i];
    return max;
}

float get_min(QVector<double> mass)
{
    float min=V_REF+1;
    for (int i=0;i<mass.length();i++)
        if (mass[i] < min) min=mass[i];
    return min;
}

float get_avg(QVector<double> mass)
{
    float avg=0;
    for (int i=0;i<mass.length();i++)
        avg+=mass[i];
    avg/=mass.length();
    return avg;
}

void MainWindow::Timer_Ovf()
{
    QVector<double> t(SAMPLES), U(SAMPLES);

    QString ADC_Data;

    if (ui->TimeRadioBtn->isChecked())
        ADC_Data=DoCommand("ADC?");
    else
        ADC_Data=DoCommand("FFT?");

    ui->TextArea->clear();

    QStringList ADC_List = ADC_Data.split(",");
    for (int i=0;i<ADC_List.length();i++)
    {
        bool ConvertOK;
        U[i]=ADC_List[i].toUInt(&ConvertOK,16);
        if (ConvertOK)
        {
            if (ui->TimeRadioBtn->isChecked())
            {
                U[i]*=V_REF/0xFFF;
                t[i]=i*10;
            }
            else
            {
                U[i]*=V_REF/0xFFFF;
                t[i]=i*Fs/SAMPLES;
            }
        }
        ui->TextArea->append(QString("%1 - %2 В ").arg(i).arg(U[i],4,'f',2));
    }
    ui->scope->graph(0)->setData(t, U);
    ui->scope->replot();
    ui->MaxLabel->setText(QString("%1В").arg(get_max(U),4,'f',2));
    ui->MinLabel->setText(QString("%1В").arg(get_min(U),4,'f',2));
    ui->AvgLabel->setText(QString("%1В").arg(get_avg(U),4,'f',2));
}

void MainWindow::on_StartBtn_clicked()
{
    if (ui->StartBtn->text()=="Старт")
    {
        timer->start(150);
        ui->StartBtn->setText("Стоп");
        ui->ConnectBtn->setEnabled(false);
    }
    else
    {
        timer->stop();
        ui->StartBtn->setText("Старт");
        ui->ConnectBtn->setEnabled(true);
    }
}

void MainWindow::mouseWheel()
{
    if (ui->scope->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->scope->axisRect()->setRangeZoom(ui->scope->xAxis->orientation());
    else if (ui->scope->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->scope->axisRect()->setRangeZoom(ui->scope->yAxis->orientation());
    else
        ui->scope->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::on_pushButton_clicked()
{
    ui->TextArea->clear();
    ui->MaxLabel->setText("0В");
    ui->MinLabel->setText("0В");
    ui->AvgLabel->setText("0В");
    ui->scope->graph(0)->clearData();
    ui->scope->replot();
}

void MainWindow::on_TimeRadioBtn_clicked()
{
    if (ui->TimeRadioBtn->isChecked())
    {
        //очистить график
        ui->scope->graph(0)->clearData();

        ui->scope->graph(0)->setBrush(QBrush(QColor(0xFF,0xFF,0xFF,0x00)));
        // задаем имена осей координат
        ui->scope->xAxis->setLabel("Время, мкс");
        ui->scope->yAxis->setLabel("Амплитуда, В");
        // задаем размеры осей
        ui->scope->xAxis->setRange(0, SAMPLES*10);
        ui->scope->yAxis->setRange(0, V_REF);
        ui->scope->replot();
    }
}

void MainWindow::on_SpectrumRadioBtn_clicked()
{
    if (ui->SpectrumRadioBtn->isChecked())
    {
        //очистить график
        ui->scope->graph(0)->clearData();
        //тип графика - палочки
        ui->scope->graph(0)->setBrush(QBrush(QColor(0x00,0x00,0xFF,0x3F)));
        // задаем имена осей координат
        ui->scope->xAxis->setLabel("Частота, Гц");
        ui->scope->yAxis->setLabel("Амплитуда, В");
        // задаем размеры осей
        ui->scope->xAxis->setRange(0, Fs);
        ui->scope->yAxis->setRange(0, 1.5);
        ui->scope->replot();
    }
}
