#ifndef YUV_GL_WIDGET_H
#define YUV_GL_WIDGET_H
#include <QGLWidget>

#include "yuv_event_listener.h"

class YUV_GL_Widget_Impl;

class YUV_GL_Widget
        : public QGLWidget
        , public Yuv_Data_Listener
{
    Q_OBJECT
public:
    explicit YUV_GL_Widget(QWidget *parent = 0);
    virtual ~YUV_GL_Widget();

    virtual int on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize );

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent*);

    bool create_video_shader();
    bool create_video_program();

signals:
    void signal_update_gl_yuv();

private slots:
    void slot_update_gl_yuv();
    void on_update_gl_yuv();

public:
    bool _inited;

private:
	YUV_GL_Widget(const YUV_GL_Widget&);
	YUV_GL_Widget& operator=(const YUV_GL_Widget&);
    char* _yuv_new[3];  //0 y 1 u 2 v

    QPoint _windowPos;
    QPoint _mousePos;
    QPoint _dPos;

    YUV_GL_Widget_Impl *_impl;
    QTimer* _timer;
};

#endif // YUV_GL_WIDGET_H
