#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDateTime>
#include <QTimer>
#include <QMovie>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置连接按钮为可切换模式
    ui->connectButton->setCheckable(true);

    // 初始化协议类型变量
    udpCombo = "";
    tcpClientCombo = "";
    tcpServerCombo = "";

    // 初始化连接对象
    client = nullptr;
    server = nullptr;
    serverClient = nullptr;
    udp = nullptr;

    // comboBox下拉列表设计
    connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(get_combo(QString)));

    // 设置默认选择第一个选项
    get_combo(ui->comboBox->currentText());

    // gif 动图
    QMovie *load =  new  QMovie("/study/gif/rainbow_cat.gif");
    ui->icon->setMovie(load); //设置动图到标签中
    qDebug() << load->scaledSize();
    load->start(); //开始播放动图

    setWindowIcon(QIcon(":/amarok.ico"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_combo(QString text)
{
    qDebug() << text;
    // 判断是什么类型
    if (text == "UDP")
    {
        ui->udpConnect->setVisible(1);
        ui->udpConnectText->setVisible(1);
        ui->udpConnectPortText->setVisible(1);
        ui->udpConnectPort->setVisible(1);
        ui->netAddress->setText("本地主机地址");
        ui->netPortText->setText("本地主机端口");
        udpCombo = text;
    }
    else if (text == "TCP Client")
    {
        ui->udpConnect->setVisible(0);
        ui->udpConnectText->setVisible(0);
        ui->udpConnectPortText->setVisible(0);
        ui->udpConnectPort->setVisible(0);
        ui->netAddress->setText("远程主机地址");
        ui->netPortText->setText("远程主机端口");
        tcpClientCombo = text;
    }
    else if (text == "TCP Server")
    {
        ui->udpConnect->setVisible(0);
        ui->udpConnectText->setVisible(0);
        ui->udpConnectPortText->setVisible(0);
        ui->udpConnectPort->setVisible(0);
        ui->netAddress->setText("本地主机地址");
        ui->netPortText->setText("本地主机端口");
        tcpServerCombo = text;
    }
}

void MainWindow::tcp_client_connect()
{
    // 1.创建客户端对象
    client = new QTcpSocket(this);

    // 关联连接成功信号
    connect(client, &QTcpSocket::connected, this, [=]()
    {
        qDebug() << "连接服务器成功";
        ui->connectButton->setText("断开连接");
        ui->connectButton->setChecked(true); });

    connect(client, &QTcpSocket::disconnected, this, [=]()
    {
        qDebug() << "断开服务器";
        ui->connectButton->setText("连接");
        ui->connectButton->setChecked(false); });

    // 2.连接服务器
    QString tcpAddress = ui->netTextArea->text();
    QString tcpPort = ui->netPortTextArea->text();
    client->connectToHost(tcpAddress, tcpPort.toUInt());
}

void MainWindow::tcp_send_data()
{
    // 3.写入数据到服务器中
    QString send_info = ui->textEdit->toPlainText();

    qDebug() << send_info;

    QString msg = send_info;

    // 获取当前日期和时间
    QDateTime now = QDateTime::currentDateTime();
    // 设置时间格式
    QString time = now.toString("yyyy-MM-dd HH:mm:ss");
    // 显示到标签
    ui->infoArea->append("Tcp Client:" + time);
    ui->infoArea->append(msg);

    client->write(send_info.toUtf8());
}

void MainWindow::tcp_read_data()
{
    // 4.读取数据
    // 关联可读信号 (只连接一次)
    connect(client, &QTcpSocket::readyRead, this, [=]()
    {
        QString msg = client->readAll();
        qDebug() << msg;
        // 获取当前日期和时间
        QDateTime now = QDateTime::currentDateTime();
        // 设置时间格式
        QString time = now.toString("yyyy-MM-dd HH:mm:ss");
        ui->infoArea->append("TCP Server:" + time);
        ui->infoArea->append(msg); });
}

void MainWindow::tcp_server_connect()
{
    // 1.创建服务器对象
    server = new QTcpServer(this);
    // 2.绑定并监听服务器
    //    bool ret = server->listen(QHostAddress::Any, 8888);
    QString tcp_server_ip = ui->netTextArea->text();
    QString tcp_server_port = ui->netPortTextArea->text();
    bool ret = server->listen(QHostAddress(tcp_server_ip), tcp_server_port.toUInt());
    if (ret)
    {
        qDebug() << "绑定成功";
    }
    else
    {
        qDebug() << "绑定失败";
    }
    // 3.关联新的连接请求信号
    connect(server, &QTcpServer::newConnection, this, [=]()
    {
        qDebug() << "有新的客户端连接";
        ui->connectButton->setText("断开连接");
        ui->connectButton->setChecked(true);
        
        // 产生新的客户端通信对象
        serverClient = server->nextPendingConnection();
        qDebug() << serverClient->peerAddress().toString(); // 解析IP
        qDebug() << serverClient->peerPort();               // 解析端口
        
        // 监听客户端断开连接信号
        connect(serverClient, &QTcpSocket::disconnected, this, [=]()
        {
            qDebug() << "客户端断开连接";
            ui->connectButton->setText("连接");
            ui->connectButton->setChecked(false);
            
            // 显示断开信息
            QDateTime now = QDateTime::currentDateTime();
            QString time = now.toString("yyyy-MM-dd HH:mm:ss");
            ui->infoArea->append("客户端断开连接:" + time);
            
            serverClient = nullptr;  // 清空客户端指针
        });
        
        // 监听客户端数据接收
        connect(serverClient, &QTcpSocket::readyRead, this, [=]()
        {
            QString msg = serverClient->readAll();
            qDebug() << "收到客户端数据:" << msg;
            
            // 显示接收到的数据
            QDateTime now = QDateTime::currentDateTime();
            QString time = now.toString("yyyy-MM-dd HH:mm:ss");
            ui->infoArea->append("TCP Client:" + time);
            ui->infoArea->append(msg);
        }); });
}


void MainWindow::udp_connect()
{
    //1.创建UDP通信对象
    udp = new QUdpSocket(this);
    
    // 2.绑定本地端口
    QString local_ip = ui->netTextArea->text();
    QString local_port = ui->netPortTextArea->text();
    
    bool ret = udp->bind(QHostAddress(local_ip), local_port.toUInt());
    if (ret)
    {
        qDebug() << "UDP绑定成功，本地地址:" << local_ip << "端口:" << local_port;
        ui->connectButton->setText("断开连接");
        ui->connectButton->setChecked(true);
        
        // 显示绑定成功信息
        QDateTime now = QDateTime::currentDateTime();
        QString time = now.toString("yyyy-MM-dd HH:mm:ss");
        ui->infoArea->append("UDP绑定成功:" + time);
        ui->infoArea->append("本地地址:" + local_ip + ":" + local_port);
        
        // 3.监听UDP数据接收
        connect(udp, &QUdpSocket::readyRead, this, [=]()
        {
            while (udp->hasPendingDatagrams())
            {
                QByteArray datagram;
                datagram.resize(udp->pendingDatagramSize());
                QHostAddress sender;
                quint16 senderPort;
                
                udp->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
                
                QString msg = QString::fromUtf8(datagram);
                qDebug() << "收到UDP数据:" << msg << "来自:" << sender.toString() << ":" << senderPort;
                
                // 显示接收到的数据
                QDateTime now = QDateTime::currentDateTime();
                QString time = now.toString("yyyy-MM-dd HH:mm:ss");
                ui->infoArea->append("UDP接收:" + time);
                ui->infoArea->append("来自 " + sender.toString() + ":" + QString::number(senderPort));
                ui->infoArea->append(msg);
            }
        });
    }
    else
    {
        qDebug() << "UDP绑定失败";
        ui->infoArea->append("UDP绑定失败");
        ui->connectButton->setText("连接");
        ui->connectButton->setChecked(false);
    }
}

void MainWindow::on_connectButton_toggled(bool checked)
{
    if (checked) // 当按钮被按下(选中)时执行连接
    {
        if (tcpClientCombo == "TCP Client")
        {
            tcp_client_connect();
            tcp_read_data();
        }
        else if (tcpServerCombo == "TCP Server")
        {
            tcp_server_connect();
        }
        else if (udpCombo == "UDP")
        {
            udp_connect();
        }
    }
    else // 当按钮被释放(未选中)时执行断开
    {
        // 断开连接的逻辑
        if (client && client->state() == QTcpSocket::ConnectedState)
        {
            client->disconnectFromHost();
        }
        if (server && server->isListening())
        {
            // 如果有客户端连接，先断开客户端
            if (serverClient && serverClient->state() == QTcpSocket::ConnectedState)
            {
                serverClient->disconnectFromHost();
            }
            server->close();
            ui->connectButton->setText("连接");
            qDebug() << "服务器已关闭";
        }
        if (udp)
        {
            udp->abort();
            ui->connectButton->setText("连接");
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->connectButton->text() == "断开连接")
    {
        if (tcpClientCombo == "TCP Client")
        {
            tcp_send_data();
        }
        else if (tcpServerCombo == "TCP Server")
        {
            // TCP服务器端发送数据
            if (serverClient && serverClient->state() == QTcpSocket::ConnectedState)
            {
                QString send_info = ui->textEdit->toPlainText();

                // 获取当前日期和时间
                QDateTime now = QDateTime::currentDateTime();
                QString time = now.toString("yyyy-MM-dd HH:mm:ss");

                // 显示发送的数据
                ui->infoArea->append("TCP Server:" + time);
                ui->infoArea->append(send_info);

                // 发送数据到客户端
                serverClient->write(send_info.toUtf8());
                qDebug() << "服务器发送数据:" << send_info;
            }
            else
            {
                qDebug() << "没有客户端连接，无法发送数据";
                ui->infoArea->append("错误：没有客户端连接");
            }
        }
        else if (udpCombo == "UDP")
        {
            // UDP发送数据
            if (udp)
            {
                QString send_info = ui->textEdit->toPlainText();
                QString remote_ip = ui->udpConnect->text();
                QString remote_port = ui->udpConnectPort->text();

                if (remote_ip.isEmpty() || remote_port.isEmpty())
                {
                    ui->infoArea->append("错误：请填写远程主机地址和端口");
                    return;
                }

                QByteArray data = send_info.toUtf8();
                qint64 ret = udp->writeDatagram(data, QHostAddress(remote_ip), remote_port.toUInt());

                if (ret != -1)
                {
                    // 获取当前日期和时间
                    QDateTime now = QDateTime::currentDateTime();
                    QString time = now.toString("yyyy-MM-dd HH:mm:ss");

                    // 显示发送的数据
                    ui->infoArea->append("UDP发送:" + time);
                    ui->infoArea->append("目标 " + remote_ip + ":" + remote_port);
                    ui->infoArea->append(send_info);

                    qDebug() << "UDP发送成功:" << send_info << "到" << remote_ip << ":" << remote_port;
                }
                else
                {
                    qDebug() << "UDP发送失败";
                    ui->infoArea->append("UDP发送失败");
                }
            }
            else
            {
                ui->infoArea->append("错误：UDP未连接");
            }
        }
    }
    else
    {
        ui->infoArea->append("未连接");
    }
}
