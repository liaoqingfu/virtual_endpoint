#include <pthread.h>
#include <cassert>
#include <functional>
#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "video_capture/my_video_capture.h"
#include "video_render/video_render.h"
#include "video_render/yuv_widget.h"

using namespace std::placeholders;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _run(false),
    _tid(0),
    _yuv_widget(NULL),
    _render(NULL),
    _capture(NULL)
{
    ui->setupUi(this);

    _yuv_widget = new YUV_Widget(ui->frame);
    assert(_yuv_widget);
    _render = new Video_Render( _yuv_widget );
    assert(_render);
    _capture = new My_Video_Capture();
    assert(_capture);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::release()
{

}

void *MainWindow::run_thread(void *arg)
{
    MainWindow* window = reinterpret_cast<MainWindow*>(arg);
    assert(window);

    window->play_video();
    return window;
}

void MainWindow::play_video()
{
    std::function< int (int w, int h, char**, int*) > video_cb_func;
    video_cb_func = std::bind( &MainWindow::video_capture_cb, this, _1, _2, _3, _4 );

    while( _run )
    {
        (*_capture)(&video_cb_func);
    }
}

int MainWindow::video_capture_cb(int w, int h, char **yuv, int *linesize)
{
    return _render->on_yuv_data_arrive(w, h, yuv, linesize);
}


void MainWindow::on_play_btn_clicked()
{
    int rc = _capture->init();
    if(rc != 0) return;
    _yuv_widget->show();

    _run = true;
    pthread_create(&_tid, NULL, run_thread, this);
}

void MainWindow::on_stop_btn_clicked()
{
    _run = false;

    if( !_tid )
    {
        pthread_join(_tid, NULL);
        _tid = 0;
    }

    _capture->release();
    _yuv_widget->hide();
}

void MainWindow::on_brightness_slider_valueChanged(int value)
{
    _capture->set_brightness( value );
}

void MainWindow::on_hue_slider_valueChanged(int value)
{
    _capture->set_hue( value );
}

void MainWindow::on_saturation_slider_valueChanged(int value)
{
    _capture->set_saturation( value );
}

void MainWindow::on_contrast_slider_valueChanged(int value)
{
    _capture->set_contrast( value );
}

void MainWindow::on_reset_btn_clicked()
{
    ui->brightness_slider->setValue(0);
    ui->hue_slider->setValue(0);
    ui->saturation_slider->setValue(17);
    ui->contrast_slider->setValue(14);
}
