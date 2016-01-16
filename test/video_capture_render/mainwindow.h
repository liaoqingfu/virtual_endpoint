#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class YUV_Widget;
class Video_Render;
class My_Video_Capture;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void release();

private:
    void play_video();
    int video_capture_cb(int w, int h, char**yuv, int*linesize);

    static void *run_thread(void *);

private slots:
    void on_play_btn_clicked();

    void on_stop_btn_clicked();

    void on_brightness_slider_valueChanged(int value);

    void on_hue_slider_valueChanged(int value);

    void on_saturation_slider_valueChanged(int value);

    void on_contrast_slider_valueChanged(int value);

    void on_reset_btn_clicked();

private:
    Ui::MainWindow *ui;

    bool                _run;
    pthread_t           _tid;
    YUV_Widget*         _yuv_widget;
    Video_Render*       _render;
    My_Video_Capture*   _capture;
};

#endif // MAINWINDOW_H
