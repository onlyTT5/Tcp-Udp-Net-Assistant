#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void get_combo(QString text);
    void tcp_server_connect();
    void tcp_send_data();
    void tcp_read_data();
    void tcp_client_connect();
    void udp_connect();

private slots:
    void on_pushButton_clicked();

    void on_connectButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QTcpSocket *client;
    QTcpServer *server;
    QUdpSocket *udp;
    QTcpSocket *serverClient; // 用于保存服务器端的客户端连接
    QString udpCombo;
    QString tcpClientCombo;
    QString tcpServerCombo;
};
#endif // MAINWINDOW_H
