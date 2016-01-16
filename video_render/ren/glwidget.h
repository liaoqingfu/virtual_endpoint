#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QString>

#include <ace/Singleton.h>

#include <suyun_sdk/yuv_buf.h>

#include "common/render_param.h"

namespace Ui {
    class GLWidget;
}

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

public:
    void update_draw(YuvBuf *buf);
    void update_param(const Render_Param& param);

private:
    void initializeGL();
    void resizeGL(int width, int height);

    void bind();
    void unbind();
    void flush();

    void event_update_mode_param();
    void event_update_state_param();

    void create_texture_and_pbo();
    bool create_video_shader();
    bool create_video_program();
    unsigned char* read_image_yuv(const char *path);

private:
    void paintGL();

private:
    Ui::GLWidget *ui;

    GLuint      _texture_id[3];          // ID of texture [0]:y [1]:u [2]:v
    GLuint      _pbo_id[2][3];           // ID of pob attarch _texture_id;
    int         _img_width;
    int         _img_height;
    int         _screen_w;
    int         _screen_h;
    double      _texture_w;
    double      _texture_h;
    bool        _is_user_pbo;
    int         _pbo_index;
    GLuint      _yuv_shader;
    GLuint      _yuv_program;

    unsigned char*  _img_data;

    Render_Param _render_param;

signals:
    void update_draw_sig(int width, int height, unsigned char *data);

public slots:
    void update_draw_slot(int width, int height, unsigned char *data);

};

typedef ACE_Singleton<GLWidget, ACE_Recursive_Thread_Mutex> GLWidget_Singleton;

#endif // GLWIDGET_H
