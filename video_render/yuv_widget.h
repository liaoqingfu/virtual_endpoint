#ifndef YUV_WIDGET_H
#define YUV_WIDGET_H

#include <QWidget>

#include "yuv_event_listener.h"

class YUV_Widget_Impl;
class YUV_Widget
        : public QWidget
        , public Yuv_Data_Listener
{
public:

    Q_OBJECT
public:
    explicit YUV_Widget(QWidget * parent = 0);
    virtual ~YUV_Widget();
    virtual int on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize );//, int *linesize);	//interface

protected:
    void paintEvent(QPaintEvent * p);
    void changeEvent(QEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

public:
    void update_event();

signals:
    void signal_on_update_event();

private slots:
    void on_update_event();

private:
	YUV_Widget(const YUV_Widget&);
	YUV_Widget& operator=(const YUV_Widget&);
    YUV_Widget_Impl *_impl;

    QPoint _windowPos;
    QPoint _mousePos;
    QPoint _dPos;
};

#endif				// YUV_WIDGET_H
