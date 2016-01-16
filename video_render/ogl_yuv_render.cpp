#include <cstring>
#include "ogl_yuv_render.h"

#include <cstdio>
using namespace std;

const char* YUV_SHADER_SRC = "uniform sampler2D Ytex;\r\n"
                             "uniform sampler2D Utex;\r\n"
                             "uniform sampler2D Vtex;\r\n"
                             "void main(void)\r\n"
                             "{\r\n"
                             "float nx,ny,r,g,b,y,u,v;\r\n"
                             "vec4 txl,ux,vx;\r\n"
                             "nx=gl_TexCoord[0].x;\r\n"
                             "ny=gl_TexCoord[0].y;\r\n"
                             "y=texture2D(Ytex,vec2(nx,ny)).r;\r\n"
                             "u=texture2D(Utex,vec2(nx,ny)).r;\r\n"
                             "v=texture2D(Vtex,vec2(nx,ny)).r;\r\n"
                             "y=1.1643*(y-0.0625);\r\n"
                             "u=u - 0.5;\r\n"
                             "v=v - 0.5;\r\n"
                             "r=y+1.5958*v;\r\n"
                             "g=y-0.39173*u-0.81290*v;\r\n"
                             "b=y+2.017*u;\r\n"
                             "gl_FragColor=vec4(r,g,b,1.0);\r\n"
                             "}\r\n";

OGL_YUV_Render::OGL_YUV_Render()
    :_yuv_shader(0)
    ,_yuv_program(0)
    ,_is_inited(false)
    ,_video_src_w(0)
    ,_video_src_h(0)
    ,_window_w(0)
    ,_window_h(0)
    ,_is_user_pbo(false)
    ,_pbo_index(0)
    ,PBO_COUNT(6)
    ,DATA_SIZE(1920*1080*3)
    ,DATA_WIDTH(1920)
    ,DATA_HEIGHT(1080)

{
    memset(_texture_id, 0, sizeof(_texture_id));
    memset(_pbo_id, 0, sizeof(_pbo_id));
}

bool OGL_YUV_Render::init(bool user_pbo)
{
    _is_user_pbo = user_pbo;
    bool fr = create_video_shader();
    if(fr) create_video_program();
    if(fr) create_video_texture();
    if (_is_user_pbo)
        if(fr) create_video_pbo();
    return fr;
}

void OGL_YUV_Render::release()
{
    destroy_video_texture();
    destroy_video_shader();
    destroy_video_program();
    if (_is_user_pbo)
        destroy_video_pbo();
}

int OGL_YUV_Render::operator() (int w, int h, char ** yuv, int* linesize)
{
    int ww[] = {w, w / 2, w / 2};
    int hh[] = {h, h / 2, h / 2};

    if (_is_user_pbo)
    {
        _pbo_index = (_pbo_index + 1) % 2;
        int next_pbo_index = (_pbo_index + 1) % 2;

        if(w != _video_src_w || h != _video_src_h)
        {
            _video_src_w = w;
            _video_src_h = h;
            for(int i = 0; i < 3; ++i)
            {
                glBindTexture(GL_TEXTURE_2D, _texture_id[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[_pbo_index][i]);
                glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE, ww[ i ], hh[ i ], 0,
                             GL_LUMINANCE,GL_UNSIGNED_BYTE,  yuv[i]);
            }
        }

        for (int i = 0; i < 3; i++)
        {
            glBindTexture(GL_TEXTURE_2D, _texture_id[i] );
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[_pbo_index][i]);
            glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, ww[i], hh[i], GL_LUMINANCE, GL_UNSIGNED_BYTE, 0 );
        }

        int data_size[] = { w * h, w * h / 4, w * h / 4 };
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
                    memcpy(ptr, yuv[i], data_size[i]);
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
        if(w != _video_src_w || h != _video_src_h)
        {
            _video_src_w = w;
            _video_src_h = h;

            for(int i = 0; i < 3; ++i)
            {
                glBindTexture(GL_TEXTURE_2D, _texture_id[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE, ww[ i ], hh[ i ], 0,
                             GL_LUMINANCE,GL_UNSIGNED_BYTE,  yuv[i]);
            }
        }
        else
        {
            for(int i = 0; i < 3; ++i)
            {
                glBindTexture(GL_TEXTURE_2D, _texture_id[i]);
                glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, ww[ i] , hh[ i ],
                                 GL_LUMINANCE, GL_UNSIGNED_BYTE, yuv[i] );
            }
        }
    }

    glUseProgram(_yuv_program);

    glActiveTextureARB( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 0 ] );

    glActiveTextureARB( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 1 ] );

    glActiveTextureARB(GL_TEXTURE2);
    glBindTexture( GL_TEXTURE_2D, _texture_id[ 2 ] );

    glClearColor(0, 1, 0, 0.9);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin (GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f,-1.0f, 0.0f);
    glEnd ();

    return 0;
}

bool OGL_YUV_Render::create_video_shader()
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
        fprintf(stderr, "opengl shader compile log = %s\n", log );
        fflush(stderr);
        delete [] log;
        return false;
    }
    return true;
}
void OGL_YUV_Render::destroy_video_shader()
{
    glDeleteShader(_yuv_shader);
}

bool OGL_YUV_Render::create_video_program()
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
        fprintf (stderr, "opengl program link log = %s\n", log );
        fflush(stderr);
        delete [] log;
        return false;
    }
    return true;
}

void OGL_YUV_Render::destroy_video_program()
{
    glDeleteProgram(_yuv_program);
}


bool OGL_YUV_Render::create_video_texture()
{
    glGenTextures(3, _texture_id);

    int i = 0;
    glUseProgram(_yuv_program);

    /* Select texture unit 1 as the active unit and bind the U texture. */
    glActiveTexture(GL_TEXTURE1);
    i = glGetUniformLocationARB(_yuv_program, "Utex");
    glUniform1iARB(i, 1);  /* Bind Utex to texture unit 1 */

    /* Select texture unit 2 as the active unit and bind the V texture. */
    glActiveTexture(GL_TEXTURE2);
    i = glGetUniformLocationARB(_yuv_program, "Vtex");
    glUniform1iARB(i, 2);  /* Bind Vtext to texture unit 2 */

    /* Select texture unit 0 as the active unit and bind the Y texture. */
    glActiveTexture(GL_TEXTURE0);
    i = glGetUniformLocationARB(_yuv_program, "Ytex");
    glUniform1iARB(i, 0);  /* Bind Ytex to texture unit 0 */

    return true;
}

void OGL_YUV_Render::destroy_video_texture()
{
    glDeleteTextures( 3, _texture_id);
}

bool OGL_YUV_Render::create_video_pbo()
{
    fprintf(stdout, "OGL_YUV_Render _is_user_pbo = true!!!\r\n");
    fflush(stdout);

    glGenBuffersARB(PBO_COUNT, (GLuint*)_pbo_id);
    int ww[] = {DATA_WIDTH, DATA_WIDTH / 2, DATA_WIDTH / 2};
    int hh[] = {DATA_HEIGHT, DATA_HEIGHT / 2, DATA_HEIGHT / 2};

    for(int i = 0; i < 2; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _pbo_id[i][j]);
            glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, ww[j] * hh[j], 0, GL_STREAM_DRAW_ARB);
        }
    }

    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    return true;
}

void OGL_YUV_Render::destroy_video_pbo()
{
    glDeleteBuffersARB(PBO_COUNT, (GLuint*)_pbo_id);
}
