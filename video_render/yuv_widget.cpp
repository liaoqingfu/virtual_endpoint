#include <QMouseEvent>
#include <QTimer>

#include "yuv_widget.h"
#include "yuv_widget_impl.h"

YUV_Widget::YUV_Widget(QWidget * parent)
    : QWidget(parent)
{
    _impl = new YUV_Widget_Impl(this);
    QObject::connect( this, SIGNAL(signal_on_update_event()),
                      this, SLOT(on_update_event()),
                      Qt::BlockingQueuedConnection);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);

    setGeometry(0, 0, 352,288);

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),  this, SLOT(on_update_event()));
    timer->start( 20 );
}

YUV_Widget::~YUV_Widget()
{
    delete _impl;
}

int YUV_Widget::on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize )//, int *linesize)
{
    return _impl->on_yuv_data_arrive(w, h, yuv, linesize);
}

void YUV_Widget::on_update_event()
{
    this->update();
}

void YUV_Widget::paintEvent(QPaintEvent * p)
{
    _impl->paintEvent(p);
}

void YUV_Widget::update_event()
{
    emit signal_on_update_event();
}


void YUV_Widget::mousePressEvent(QMouseEvent *event)
{
    if( !parent() )
    {
        this->_windowPos = this->pos();
        this->_mousePos = event->globalPos();
        this->_dPos = _mousePos - _windowPos;
    }
}

void YUV_Widget::mouseMoveEvent(QMouseEvent *event)
{
    if( !parent() )
    {
        this->move(event->globalPos() - this->_dPos);
    }
}

void YUV_Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
}
