#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ep_interface.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Ep_Interface::instance()->video_decode_init(ui->frame);
    Ep_Interface::instance()->video_encode_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_encode_pbt_clicked()
{
    int ip = inet_addr("127.0.0.1");
    Ep_Interface::instance()->video_encode_start(6666, ip);
}

void MainWindow::on_stop_encode_pbt_clicked()
{
    Ep_Interface::instance()->video_encode_stop();;
}

void MainWindow::on_start_decode_pbt_clicked()
{
    int ip = inet_addr("127.0.0.1");
    Ep_Interface::instance()->video_decode_start(6666, ip);
}

void MainWindow::on_stop_decode_pbt_clicked()
{
    Ep_Interface::instance()->video_decode_stop();
}
