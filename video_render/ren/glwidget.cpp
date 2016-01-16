#include <QDesktopWidget>
#include <QImage>


#include <GL/glew.h>

#if defined (_WIN32)

#include <GL/wglew.h>

#endif

#if defined(__APPLE__) || defined(MACOSX)

#include <GLUT/glut.h>

#else

#include <GL/glut.h>

#endif

#include "glwidget.h"
#include "ui_glwidget.h"
#include "glInfo.h"

#include "video_render_defines.h"
#include "video_shader.h"
#include  "cli/cli_object/device_types.h"

GLWidget::GLWidget(QWidget *parent) :
    _pbo_index(0),
    _is_user_pbo(true),
    _yuv_shader(0), _yuv_program(0),
    _render_param(RENDER_FULL_MODE, DEVICE_RENDER_BUSY_STATE),
    QGLWidget(parent),
    ui(new Ui::GLWidget)
{
    ui->setupUi(this);

    _screen_w = QApplication::desktop()->width();
    _screen_h = QApplication::desktop()->height();

    _img_width = 1920;
    _img_height = 1080;

    _texture_w = _img_width / _screen_w;
    _texture_h = _img_height / _screen_h;

    Qt::WindowFlags flags = 0;
    flags |= Qt::FramelessWindowHint;
    this->setWindowFlags(flags);
    this->setFixedSize(_screen_w, _screen_h);

    this->setGeometry(0, 0, _screen_w, _screen_h );

    this->resize(_screen_w, _screen_h);
    this->resizeGL(_screen_w, _screen_h);

    _img_data = read_image_yuv("./bk.yuv");

    memset(_texture_id, 0, sizeof(_texture_id));
    memset(_pbo_id, 0, sizeof(_pbo_id));

    connect(this, SIGNAL(update_draw_sig(int, int, unsigned char *)), this, SLOT(update_draw_slot( int, int, unsigned char *)));
}

GLWidget::~GLWidget()
{
    if (_img_data != NULL)
    {
        delete [] _img_data;
        _img_data = NULL;
    }
    delete ui;
}

void GLWidget::initializeGL()
{
    makeCurrent();

    QGLWidget::initializeGL();

    glEnable(GL_TEXTURE_2D);

    GLenum ggg = glewInit();

    // get OpenGL info
    glInfo glInfo;
    glInfo.getInfo();
    glInfo.printSelf();

    // for linux, do not need to get function pointers, it is up-to-date
    if(glInfo.isExtensionSupported("GL_ARB_pixel_buffer_object"))
    {
        _is_user_pbo = true;
        printf("Video card supports GL_ARB_pixel_buffer_object.\n");
    }
    else
    {
        _is_user_pbo = false;
        printf("Video card does NOT support GL_ARB_pixel_buffer_object.\n");
    }

    create_texture_and_pbo();

    if(!create_video_shader())
    {
        fprintf(stderr, "create_video_shader false\n");
    }
    if(!create_video_program())
    {
        fprintf(stderr, "create_video_program false\n");
    }

}

unsigned char * GLWidget::read_image_yuv(const char *path)
{
    FILE *fp;
    unsigned char * buffer;
    long size = 1920 * 1080 * 3 / 2;

    if((fp=fopen(path,"rb"))==NULL)
    {
        fprintf(stderr, "open file fail\n");
        exit(0);
    }

    buffer = new unsigned char[size];
    memset(buffer,'\0',size);
    long len = fread(buffer,1,size,fp);
    fclose(fp);
    return buffer;
}

void GLWidget::create_texture_and_pbo()
{
    glGenTextures(3, _texture_id);
    printf("texture : %d -- %d -- %d---w(%d %d)---------------\n",
           _texture_id[0], _texture_id[1], _texture_id[2], _img_width, _img_height);

    int ww[] = {_img_width, _img_width / 2, _img_width / 2};
    int hh[] = {_img_height, _img_height / 2, _img_height / 2};
    for(int i = 0; i < 3; ++i)
    {
        glBindTexture(GL_TEXTURE_2D,_texture_id[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE, ww[ i ], hh[ i ],0,GL_LUMINANCE,GL_UNSIGNED_BYTE,  0);
    }

    glBindTexture(GL_TEXTURE_2D, _texture_id[0] );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 1920, 1080, GL_LUMINANCE, GL_UNSIGNED_BYTE, _img_data );

    glBindTexture(GL_TEXTURE_2D, _texture_id[1]);
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 1920/2, 1080/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, _img_data + 1920 * 1080);

    glBindTexture(GL_TEXTURE_2D, _texture_id[2]);
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 1920/2, 1080/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, _img_data + 1920 * 1080 * 5 / 4 );

    if(_is_user_pbo)
    {
        glGenBuffersARB(6, (GLuint*)_pbo_id);
        for(int i = 0; i < 2; ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[i][j]);
                glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, ww[j] * hh[j], 0, GL_STREAM_DRAW_ARB);
            }
        }
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLWidget::paintGL()
{
    //glEnable(GL_TEXTURE_2D);

    //glClear (GL_RED_BITS);
    //glDisable(GL_BLEND);
    int i = 0;
    glUseProgram(_yuv_program);

    /* Select texture unit 1 as the active unit and bind the U texture. */
    glActiveTexture(GL_TEXTURE1);
    i = glGetUniformLocationARB(_yuv_program, "Utex");
    //printf("\tU:%d", i);
    glUniform1iARB(i, 1);  /* Bind Utex to texture unit 1 */

    /* Select texture unit 2 as the active unit and bind the V texture. */
    glActiveTexture(GL_TEXTURE2);
    i = glGetUniformLocationARB(_yuv_program, "Vtex");
    //printf("\tV:%d", i);
    glUniform1iARB(i, 2);  /* Bind Vtext to texture unit 2 */

    /* Select texture unit 0 as the active unit and bind the Y texture. */
    glActiveTexture(GL_TEXTURE0);
    i = glGetUniformLocationARB(_yuv_program, "Ytex");
    //printf("\tY:%d\n", i);
    glUniform1iARB(i, 0);  /* Bind Ytex to texture unit 0 */


    //glEnable (GL_TEXTURE_2D); /* enable texture mapping */

    double x_0 = 0.0 - _texture_w;
    double y_0 = 0.0 - _texture_h;
    double x_1 = 0.0 + _texture_w;
    double y_1 = 0.0 + _texture_h;

    bind();
    glClearColor(0,0,0,0);
    glClear (GL_COLOR_BUFFER_BIT);

    glBegin (GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);   glVertex2f(x_0,y_1);
    glTexCoord2f(1.0f, 0.0f);   glVertex2f(x_1,y_1);
    glTexCoord2f(1.0f, 1.0f);   glVertex2f(x_1,y_0);
    glTexCoord2f(0.0f, 1.0f);   glVertex2f(x_0,y_0);
    //    glTexCoord2f(0.0f, 0.0f);   glVertex2f(-1.0f, 1.0f);
    //    glTexCoord2f(1.0f, 0.0f);   glVertex2f(1.0f, 1.0f);
    //    glTexCoord2f(1.0f, 1.0f);   glVertex2f(1.0f, -1.0f);
    //    glTexCoord2f(0.0f, 1.0f);   glVertex2f(-1.0f, -1.0f);

    glEnd ();

    unbind();

    glUseProgram(0);

}

void GLWidget::update_draw(YuvBuf *buf)
{
    if (_render_param.render_state == DEVICE_RENDER_BUSY_STATE)
    {
        unsigned char *out_buf = (unsigned char*)buf->buf();
        int t_int = sizeof(int);
        int w = *((int*)out_buf);
        int h = *((int*)(out_buf+t_int));

        emit(update_draw_sig(w, h, out_buf+t_int*3));
    }
}

void GLWidget::update_param(const Render_Param& param)
{
    if (param.render_mode != _render_param.render_mode)
    {
        _render_param.render_mode = param.render_mode;
        event_update_mode_param();
    }
    if (param.render_state != _render_param.render_state)
    {
        _render_param.render_state = param.render_state;
        event_update_state_param();
    }
}

void GLWidget::event_update_state_param()
{
    if (_render_param.render_state == DEVICE_RENDER_IDLE_STATE)
    {
        int count = 2;
        while(count--)
        {
            emit(update_draw_sig(1920, 1080, _img_data));
        }
    }
}

void GLWidget::event_update_mode_param()
{
    switch (_render_param.render_mode) {
    case RENDER_FULL_MODE:
    {
        _texture_w = (double)_screen_w / _screen_w;
        _texture_h = (double)_screen_h / _screen_h;
        break;
    }
    case RENDER_NARROW_MODE:
    {
        if (_screen_h > _screen_w)
        {
            _texture_h = 1.0;
            _texture_w = (double)_screen_w/_screen_h;
        }
        if (_screen_w > _screen_h)
        {
            _texture_w = 1.0;
            _texture_h = (double)_screen_h/_screen_w;
        }

        break;
    }
    case RENDER_SCALE_MODE:
    {
        if (_img_width > _screen_w)
        {
            _texture_w = 1.0;
        }
        else
        {
            _texture_w = (double)_img_width/_screen_w;
        }
        if (_img_height > _screen_h)
        {
            _texture_h = 1.0;
        }
        else
        {
            _texture_h = (double)_img_height/_screen_h;
        }

        break;
    }
    default:
        break;
    }
}

void GLWidget::update_draw_slot(int width, int height,
                                unsigned char *data)
{
    makeCurrent();

    if (_img_width != width || _img_height != height)
    {
        _img_width = width;
        _img_height = height;

        flush();
        create_texture_and_pbo();

        create_video_shader();

        create_video_program();
    }
    if (_is_user_pbo)
    {
        _pbo_index = (_pbo_index + 1) % 2;
        int next_pbo_index = (_pbo_index + 1) % 2;

        glBindTexture(GL_TEXTURE_2D, _texture_id[0] );
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[_pbo_index][0]);
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0 );

        glBindTexture(GL_TEXTURE_2D, _texture_id[1]);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[_pbo_index][1]);
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

        glBindTexture(GL_TEXTURE_2D, _texture_id[2]);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[_pbo_index][2]);
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);

        unsigned char* data_ptr[] = { data, data + width * height, data + width * height * 5 / 4};
        int data_size[] = { width * height, width * height / 4, width * height / 4 };
        for(int i = 0; i < 3; ++i)
        {
            // bind PBO to update pixel values
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[next_pbo_index][i]);

            // map the buffer object into client's memory
            // Note that glMapBufferARB() causes sync issue.
            // If GPU is working with this buffer, glMapBufferARB() will wait(stall)
            // for GPU to finish its job. To avoid waiting (stall), you can call
            // first glBufferDataARB() with NULL pointer before glMapBufferARB().
            // If you do that, the previous data in PBO will be discarded and
            // glMapBufferARB() returns a new allocated pointer immediately
            // even if GPU is still working with the previous data.
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, data_size[i], 0, GL_STREAM_DRAW_ARB);
            GLubyte* ptr = (GLubyte*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

            if(ptr)
            {
                // update data directly on the mapped buffer
                try
                {
                    memcpy(ptr, data_ptr[i], data_size[i]);
                    //memset(ptr, (i + 1) * 44, data_size[i]);
                }
                catch(...)
                {
                }

                glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB); // release pointer to mapping buffer
            }
            else
            {
                printf("ptr == NULL\n");
            }
        }
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _texture_id[0] );
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data );

        glBindTexture(GL_TEXTURE_2D, _texture_id[1]);
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width * height);

        glBindTexture(GL_TEXTURE_2D, _texture_id[2]);
        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width/2, height/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, data + width * height * 5 / 4 );
    }
    //delete [] data;
    updateGL();
}

bool GLWidget::create_video_shader()
{
    _yuv_shader = glCreateShader(GL_FRAGMENT_SHADER);

    //const char* shader_src = "uniform sampler2DRect Ytex;\r\n uniform sampler2DRect Utex,Vtex;\r\n void main(void)\r\n {\r\n float nx,ny,r,g,b,y,u,v;\r\n vec4 txl,ux,vx;\r\n nx=gl_TexCoord[0].x;\r\n ny=gl_TexCoord[0].y;\r\n y=texture2DRect(Ytex,vec2(nx,ny)).r;\r\n u=texture2DRect(Utex,vec2(nx/2.0,ny/2.0)).r;\r\n v=texture2DRect(Vtex,vec2(nx/2.0,ny/2.0)).r;\r\n y=1.1643*(y-0.0625);\r\n u=u - 0.5;\r\n v=v - 0.5;\r\n r=y+1.5958*v;\r\ng=y-0.39173*u-0.81290*v;\r\n b=y+2.017*u;\r\n gl_FragColor=vec4(r,g,b,1.0);\r\n }\r\n";
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
        ACE_DEBUG ( ( LM_DEBUG, ACE_TEXT ( "opengl shader compile log = %s\n" ), log ) );
        delete [] log;
        return false;
    }
    return true;
}

bool GLWidget::create_video_program()
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
        ACE_DEBUG ( ( LM_DEBUG, ACE_TEXT ( "opengl program link log = %s\n" ), log ) );
        delete [] log;
        return false;
    }
    return true;
}

void GLWidget::bind()
{
    glActiveTextureARB( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 0 ] );

    glActiveTextureARB( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 1 ] );

    glActiveTextureARB(GL_TEXTURE2);
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 2 ] );
}

void GLWidget::unbind()
{
    glActiveTextureARB( GL_TEXTURE2 );
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB( GL_TEXTURE0 );
    glDisable(GL_TEXTURE_2D);
}

void GLWidget::flush()
{
    glDeleteProgram(_yuv_program);
    glDeleteShader(_yuv_shader);
    glDeleteTextures(3, _texture_id);
    if(_is_user_pbo)
    {
        glDeleteBuffersARB(6, (GLuint*)_pbo_id);
    }

}
