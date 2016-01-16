#include <QTimer>
#include <QMouseEvent>

#include <GL/glew.h>

#include "yuv_gl_widget.h"
#include "yuv_shader_src.h"
#include "glInfo.h"

#include "yuv_gl_widget_impl.h"


#include <cstdio>

YUV_GL_Widget::YUV_GL_Widget(QWidget *parent)
    : QGLWidget(parent)
    , _inited( false )
    ,_timer(NULL)
{
    _impl = new YUV_GL_Widget_Impl(this);

//    QObject::connect( this, SIGNAL(signal_update_gl_yuv()),
//                      this, SLOT(slot_update_gl_yuv()),
//                      Qt::AutoConnection);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()),  this, SLOT(on_update_gl_yuv()));
    _timer->start( 50 );

    setGeometry(0, 0, 352,288);

    setAutoBufferSwap(false);
}

YUV_GL_Widget::~YUV_GL_Widget()
{
    delete _timer;
    delete _impl;
    _timer = NULL;
    _impl = NULL;
}

void YUV_GL_Widget::on_update_gl_yuv()
{
    this->update();
    //    emit signal_update_gl_yuv();
}


void YUV_GL_Widget::slot_update_gl_yuv()
{
    this->_impl->paintGL();
}


int YUV_GL_Widget::on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize )
{
    return this->_impl->on_yuv_data_arrive(w, h, yuv, linesize);
}

void YUV_GL_Widget::initializeGL()
{
    QGLWidget::initializeGL();

    glewInit();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
    glDisable( GL_DEPTH_TEST );

    this->_impl->create_video_shader();
    this->_impl->create_video_program();

    glGenTextures(3, this->_impl->_texture_id);

    int i = 0;
    glUseProgram(this->_impl->_yuv_program);

    /* Select texture unit 1 as the active unit and bind the U texture. */
    glActiveTexture(GL_TEXTURE1);
    i = glGetUniformLocationARB(this->_impl->_yuv_program, "Utex");
    glUniform1iARB(i, 1);  /* Bind Utex to texture unit 1 */

    /* Select texture unit 2 as the active unit and bind the V texture. */
    glActiveTexture(GL_TEXTURE2);
    i = glGetUniformLocationARB(this->_impl->_yuv_program, "Vtex");
    glUniform1iARB(i, 2);  /* Bind Vtext to texture unit 2 */

    /* Select texture unit 0 as the active unit and bind the Y texture. */
    glActiveTexture(GL_TEXTURE0);
    i = glGetUniformLocationARB(this->_impl->_yuv_program, "Ytex");
    glUniform1iARB(i, 0);  /* Bind Ytex to texture unit 0 */

    glClear( GL_COLOR_BUFFER_BIT );

    this->doneCurrent();
    _inited = true;
}

void YUV_GL_Widget::resizeGL(int width, int height)
{
    makeCurrent();

    glViewport( 0, 0, GLsizei( width ), GLsizei( height ) );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( 0, width, height, 0 );

    doneCurrent();

    printf( "11111111111 resizegl w %d h %d \n", width, height);
}

void YUV_GL_Widget::paintGL()
{
    makeCurrent();

    glClear( GL_COLOR_BUFFER_BIT );

    swapBuffers();
    doneCurrent();

    return;
}

void YUV_GL_Widget::paintEvent(QPaintEvent* )
{
    this->_impl->paintGL();
}

void YUV_GL_Widget::mousePressEvent(QMouseEvent *event)
{
    if( !parent() )
    {
        this->_windowPos = this->pos();
        this->_mousePos = event->globalPos();
        this->_dPos = _mousePos - _windowPos;
    }
}

void YUV_GL_Widget::mouseMoveEvent(QMouseEvent *event)
{
    if( !parent() )
    {
        this->move(event->globalPos() - this->_dPos);
    }
}
