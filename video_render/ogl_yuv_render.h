#ifndef OGL_YUV_RENDER_H
#define OGL_YUV_RENDER_H

#include <GL/glxew.h>

class OGL_YUV_Render
{
public:
    OGL_YUV_Render();
    bool init(bool user_pbo = false);
    void release();
    int operator() (int w, int h, char ** yuv, int* linesize);

private:
	OGL_YUV_Render(const OGL_YUV_Render&);
	OGL_YUV_Render& operator=(const OGL_YUV_Render&);
    bool create_video_program();
    void destroy_video_program();
    bool create_video_shader();
    void destroy_video_shader();
    bool create_video_texture();
    void destroy_video_texture();
    bool create_video_pbo();
    void destroy_video_pbo();

private:
    GLuint  _yuv_shader;
    GLuint  _yuv_program;
    bool    _is_inited;
    int     _video_src_w;
    int     _video_src_h;
    int     _window_w;
    int     _window_h;
    GLuint  _texture_id[3];          // ID of texture [0]:y [1]:u [2]:v
    bool    _is_user_pbo;
    GLuint  _pbo_id[2][3];
    int     _pbo_index;
    const int PBO_COUNT;
    const int DATA_SIZE;
    const int DATA_WIDTH;
    const int DATA_HEIGHT;


};

#endif // OGL_YUV_RENDER_H
