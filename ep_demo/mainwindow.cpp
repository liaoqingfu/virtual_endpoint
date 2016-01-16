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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_encdeo_pbt_clicked()
{
    int ip = inet_addr("127.0.0.1");
    Ep_Interface::instance()->video_decode_start(8888, ip);
}

void MainWindow::on_stop_encode_pbt_clicked()
{
    Ep_Interface::instance()->video_decode_stop();
}
