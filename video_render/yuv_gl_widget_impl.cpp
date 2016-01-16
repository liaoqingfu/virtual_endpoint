#include <GL/glew.h>

#include <cstdio>
#include "yuv_gl_widget_impl.h"
#include "yuv_gl_widget.h"
#include "yuv_shader_src.h"
#include "glInfo.h"

YUV_GL_Widget_Impl::YUV_GL_Widget_Impl(YUV_GL_Widget *widget)
    :_widget(widget)
    , _video_src_w( 0 )
    , _video_src_h( 0 )
    ,_prev_data(NULL)
{
    _yuv_queue.open();
    memset(_texture_id, 0, sizeof( _texture_id ) );
}

YUV_GL_Widget_Impl::~YUV_GL_Widget_Impl()
{
    _destory_mutex.acquire();
    _destory_mutex.release();
    if(_prev_data){
        delete _prev_data;
        _prev_data = NULL;
    }
}

int YUV_GL_Widget_Impl::on_yuv_data_arrive(int w, int h, char **yuv, int *linesize)
{
    if( !this->_widget->_inited )
    {
        return 0;
    }

    YUV_Data *yuv_data = new YUV_Data();
    _destory_mutex.acquire();

    yuv_data->width = w;
    yuv_data->height = h;
//    yuv_data->linesize = linesize;
    yuv_data->buffer[0] = new char[ w * h];
    yuv_data->buffer[1] = new char[ w * h / 4 ];
    yuv_data->buffer[2] = new char[ w * h / 4 ];

    for( int i = 0; i < h; ++i )
    {
        memcpy( yuv_data->buffer[0] + i * w, yuv[0] + i * linesize[0], w );
    }
    for( int i = 0; i < h / 2; ++i )
    {
        memcpy( yuv_data->buffer[1] + i * w / 2, yuv[1] + i * linesize[1], w / 2 );
    }
    for( int i = 0; i < h / 2; ++i )
    {
        memcpy( yuv_data->buffer[2] + i * w / 2, yuv[2] + i * linesize[2], w / 2 );
    }



    _yuv_queue.enqueue(yuv_data);
    _destory_mutex.release();

    return 0;
}


void YUV_GL_Widget_Impl::paintGL()
{
    if(_yuv_queue.is_empty())
    {
        if(_prev_data)
            draw_GL(_prev_data);
        return;
    }
    //printf( "YUV_Widget::paintEvent(), queue size  long =%d .\n", _listSize);

    YUV_Data *yuv_data = NULL;
    while (true)
    {
        int rc = _yuv_queue.dequeue(yuv_data);
        //printf ("---------------------rc = %d\n", rc);

        if(rc > 2)
        {
            if( _prev_data )
                delete _prev_data;
            _prev_data = yuv_data;
            continue;
        }

        if(rc >= 0)
        {
            draw_GL(yuv_data);

            //fprintf( stderr, "222222222222222222222222222222222222\n");
            if( _prev_data )
                delete _prev_data;
            _prev_data = yuv_data;
        }
        else
        {
            break;
        }

        if(rc == 0) break;
    }

    return;
}

void YUV_GL_Widget_Impl::draw_GL(YUV_Data *yuv_new)
{
    this->_widget->makeCurrent();

    //    glMatrixMode(GL_PROJECTION);
    //    glLoadIdentity();
    //    gluOrtho2D( 0, width(), height(), 0 );

    //    glMatrixMode( GL_MODELVIEW );
    //    glLoadIdentity();
    int w = yuv_new->width;
    int h = yuv_new->height;

    int ww[] = {w, w / 2, w / 2};
    int hh[] = {h, h / 2, h / 2};
    if( _video_src_w != w || _video_src_h != h )
    {
        _video_src_w = w;
        _video_src_h = h;

        //resize to new video width and video height
        for(int i = 0; i < 3; ++i)
        {
            glBindTexture(GL_TEXTURE_2D,_texture_id[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE, ww[ i ], hh[ i ], 0,
                         GL_LUMINANCE,GL_UNSIGNED_BYTE,  yuv_new->buffer[i]);
        }
    }
    else
    {
        for(int i = 0; i < 3; ++i)
        {
            glBindTexture(GL_TEXTURE_2D,_texture_id[i]);
            glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, ww[ i] , hh[ i ],
                             GL_LUMINANCE, GL_UNSIGNED_BYTE, yuv_new->buffer[i] );
        }
    }


    //glEnable (GL_TEXTURE_2D); /* enable texture mapping */

    double x_0 = 0;
    double y_0 = 0;
    double x_1 = this->_widget->width();
    double y_1 = this->_widget->height();

    //    glEnable( GL_TEXTURE_2D );
    glUseProgram(_yuv_program);

    glActiveTextureARB( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 0 ] );

    glActiveTextureARB( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 1 ] );

    glActiveTextureARB(GL_TEXTURE2);
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 2 ] );

    glBegin (GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);   glVertex2f(x_0,y_1);
    glTexCoord2f(1.0f, 1.0f);   glVertex2f(x_1,y_1);
    glTexCoord2f(1.0f, 0.0f);   glVertex2f(x_1,y_0);
    glTexCoord2f(0.0f, 0.0f);   glVertex2f(x_0,y_0);
    glEnd ();

    //    glActiveTextureARB( GL_TEXTURE2 );
    //    glDisable(GL_TEXTURE_2D);

    //    glActiveTextureARB(GL_TEXTURE1);
    //    glDisable(GL_TEXTURE_2D);

    //    glActiveTextureARB( GL_TEXTURE0 );
    //    glDisable(GL_TEXTURE_2D);

    //    glUseProgram(0);

    this->_widget->swapBuffers();
    this->_widget->doneCurrent();
    //this->updateGL();
}


bool YUV_GL_Widget_Impl::create_video_shader()
{
    _yuv_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* shader_src[] = { YUV_SHADER_SRC };
    glShaderSource(_yuv_shader, 1, shader_src, NULL); //fragment_shader_src
    glCompileShader(_yuv_shader); //编译着色器对象shader的源码

    GLint compiled = 0;
    glGetShaderiv(_yuv_shader, GL_COMPILE_STATUS, &compiled);
    if(compiled == 0)
    {
        GLint length = 0;
        GLchar *log = 0;
        glGetShaderiv(_yuv_shader, GL_INFO_LOG_LENGTH, &length);
        log = new GLchar [length];
        glGetShaderInfoLog(_yuv_shader, length, &length, log);
        printf( "opengl shader compile log = %s\n", log );
        delete [] log;
        return false;
    }
    return true;
}

bool YUV_GL_Widget_Impl::create_video_program()
{
    _yuv_program = glCreateProgram();
    glAttachShader(_yuv_program, _yuv_shader);
    glLinkProgram(_yuv_program);   //对链接到着色器程序的所有着色器对象进行处理，生成完整的着色器程序

    GLint linked = 0;
    glGetProgramiv(_yuv_program, GL_LINK_STATUS, &linked);

    if(linked == 0)
    {
        GLint length = 0;
        GLchar *log = 0;
        glGetProgramiv(_yuv_program, GL_INFO_LOG_LENGTH, &length);

        log = new GLchar [length];
        glGetProgramInfoLog(_yuv_program, length, &length, log);
        printf ( "opengl program link log = %s\n", log );
        delete [] log;
        return false;
    }
    return true;
}
