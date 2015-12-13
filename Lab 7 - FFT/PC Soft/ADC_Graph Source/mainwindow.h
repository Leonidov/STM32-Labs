#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString DoCommand(QString cmd);

private slots:
    void readData();
    void Timer_Ovf();
    void on_Menu_Close_triggered();
    void on_action_triggered();
    void on_ConnectBtn_clicked();
    void on_StartBtn_clicked();
    void mouseWheel();

    void on_pushButton_clicked();

    void on_TimeRadioBtn_clicked();

    void on_SpectrumRadioBtn_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    void SendData(QString data);
};

#endif // MAINWINDOW_H
