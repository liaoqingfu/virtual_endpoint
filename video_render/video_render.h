#ifndef VIDEO_RENDER_H
#define VIDEO_RENDER_H

#include <inttypes.h>
#include <sys/types.h>

#include "yuv_event_listener.h"

class YUV_Widget;
class YUV_GL_Widget;
class OGL_Widget;

class Video_Render : public Yuv_Data_Listener
{
public:
    explicit Video_Render(YUV_Widget* yuv_widget = 0);
    explicit Video_Render(YUV_GL_Widget* yuv_widget = 0);
    explicit Video_Render(OGL_Widget* ogl_widget = 0);
    ~Video_Render();

    /**
     * @brief on_yuv_data_arrive 渲染视频数据
     * @param w 视频图像宽度
     * @param h 视频图像高度
     * @param yuv 视频数据
     * @param linesize 数组指针，linesize[0]表示Y分量的宽度，linesize[1]表示U分量的宽度，linesize[2]表示V分量的宽度
     * @return 0:成功 -1:失败
     */
    virtual int on_yuv_data_arrive(int w, int h, char **yuv, int* linesize);

    /**
     * @brief get_yuv_widget 获取qt渲染窗口类指针
     * @return NULL:失败
     */
    YUV_Widget *get_yuv_widget();

    /**
     * @brief get_yuv_gl_widget 获取qt-opengl渲染窗口类指针
     * @return NULL:失败
     */
    YUV_GL_Widget *get_yuv_gl_widget();

    /**
     * @brief get_ogl_widget 获取opengl渲染窗口类指针
     * @return NULL:失败
     */
    OGL_Widget *get_ogl_widget();

private:
    /**
     * @brief _yuv_widget qt渲染窗口类指针
     */
    YUV_Widget* _yuv_widget;

    /**
     * @brief _yuv_gl_widget qt-opengl渲染窗口类指针
     */
    YUV_GL_Widget* _yuv_gl_widget;
    /**
     * @brief _ogl_widget opengl渲染窗口类指针
     */
    OGL_Widget* _ogl_widget;
private:
	Video_Render(const Video_Render&);
	Video_Render& operator=(const Video_Render&);
};

#endif // VIDEO_RENDER_H
