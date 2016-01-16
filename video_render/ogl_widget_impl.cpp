#include <cstdio>
#include <algorithm>
#include <ace/Guard_T.h>

#include "ogl_widget_impl.h"
#include "ogl_yuv_render.h"

//#define SINGLE_THREAD
OGL_Widget_Impl::OGL_Widget_Impl(unsigned long parent_win_id)
    :_parent_win_id(-1)
    ,_painter_pid(0)
    ,_win_event_pid(-1)
    ,_is_inited(false)
    ,_is_run_painter(false)
    , _old_width(0)
    ,_old_height(0)
    ,_display(NULL)
    ,_current_win_id(0)
    ,_render(NULL)
    ,_win_change(NULL)
    ,_win_attr(NULL)
    ,_is_stoped(true)
{
    _yuv_queue.open();
    _parent_win_id = parent_win_id;
}

bool OGL_Widget_Impl::init()
{
    _is_run_painter = true;
#ifdef  SINGLE_THREAD
    this->painter_init();
#else
    this->run_painter();
#endif

    return true;
}

void OGL_Widget_Impl::release()
{
#ifdef SINGLE_THREAD
    _render->release();
#endif

    _yuv_queue.deactivate();
    _is_run_painter = false;
    _is_inited = false;
    while(!_is_stoped)
    {
        printf("wait  paint to exit\n");
        usleep(1000);
    }
    XDestroyWindow(_display, _current_win_id);
    if (_display)
        XCloseDisplay(_display);
}

OGL_Widget_Impl::~OGL_Widget_Impl()
{
}

bool OGL_Widget_Impl::run_painter()
{
    if (0 == _painter_pid)
        pthread_create(&_painter_pid, NULL, painter, reinterpret_cast<void*>(this));
    _is_stoped = false;
    return true;
}

int OGL_Widget_Impl::painter_func()
{
    _display = XOpenDisplay(":0");
    if(_display == NULL)
    {
        ACE_OS::fprintf(stderr, "open display error!\n");
        fflush(stderr);
        return -1;
    }

    ACE_OS::fprintf(stdout, "open display success!\n");
    fflush(stdout);


    _current_win_id = XCreateSimpleWindow(_display
                                          , _parent_win_id
                                          , 0, 0, 100, 100
                                          , 0, WhitePixel(_display, 0)
                                          , BlackPixel(_display, 0));


    ACE_OS::fprintf(stdout, "create win (id = %lu) from parent(id = %lu)!\n"
                    , _current_win_id, _parent_win_id);

    ACE_OS::fflush(stdout);

    int attrListSgl[] =
    { GLX_RGBA, GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4, GLX_BLUE_SIZE, 4,
      GLX_DEPTH_SIZE, 16, None };
    int attrListDbl[] =
    { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4,
      GLX_BLUE_SIZE, 4, GLX_DEPTH_SIZE, 16, None };
    int screen = DefaultScreen(_display);
    XVisualInfo* visual = glXChooseVisual(_display, screen, attrListDbl);
    if (NULL == visual)
    {
        visual = glXChooseVisual(_display, screen, attrListSgl);
        fprintf(stdout, "Singlebuffered : true\n");
        if(NULL == visual)
        {
            fprintf(stdout, "Could not get suitable XVisualInfo\n");
            return -1;
        }
    }
    else
    {
        fprintf(stdout, "Doublebuffered : true\n");
    }
    // Create the rendering context
    _glcontext = glXCreateContext(_display, visual, 0, GL_TRUE);
    // Create a colormap
    Colormap colormap = XCreateColormap
                        (_display, RootWindow(_display, visual->screen),
                         visual->visual, AllocNone);

    // Make the rendering context current, perform initialization, then
    // deselect it
    if(False == glXMakeCurrent(_display, _current_win_id, _glcontext))
    {
        fprintf(stdout, "glXMakeCurrent failed!\n");
        return -1;
    }

    GLenum err = glewInit();
    if(err != 0)
    {
        GLubyte const *err_str = glewGetErrorString(err);
        fprintf(stderr, "%s\n", err_str);
        return -1;
    }

    OGL_YUV_Render render;

    if(!render.init(false))
    {
        fprintf(stderr, "OGL_YUV_Render init error!\n");
        return -1;
    }

    XMapWindow(_display, _current_win_id);
    XFlush(_display);
    XSync(_display, true);

    _is_inited = true;

    XWindowChanges win_change;
    XWindowAttributes win_attr;

    OGL_WIDGET_IMPL::YUV_Data* yuv_data = NULL;
    while (_is_run_painter)
    {

        if(True == XGetWindowAttributes(_display, _parent_win_id, &win_attr))
        {
            if ((_old_width != win_attr.width) ||
                    ( _old_height != win_attr.height))
            {
                unsigned int vmask = CWWidth | CWHeight;
                win_change.width = win_attr.width;
                win_change.height = win_attr.height;
                XConfigureWindow(_display, _current_win_id, vmask, &win_change);
                glViewport(0, 0, win_attr.width, win_attr.height);
                _old_width = win_attr.width;
                _old_height = win_attr.height;
            }
        }

        _yuv_queue.dequeue(yuv_data);
        if (NULL == yuv_data)
        {
            fprintf(stdout, "_yuv_queue.dequeue() NULL!!!!!\r\n");
            fflush(stdout);
            continue;
        }

        render(yuv_data->width, yuv_data->height, yuv_data->buffer, NULL);
        glXSwapBuffers(_display, _current_win_id);

        delete yuv_data;
        yuv_data = NULL;
    }
    glXMakeCurrent(_display, None, NULL);
    render.release();
    _is_stoped = true;
    return 0;
}

int OGL_Widget_Impl::painter_init()
{
    _display = XOpenDisplay(":0");
    if(_display == NULL)
    {
        ACE_OS::fprintf(stderr, "open display error!\n");
        fflush(stderr);
        return -1;
    }

    ACE_OS::fprintf(stdout, "open display success!\n");
    fflush(stdout);


    _current_win_id = XCreateSimpleWindow(_display
                                          , _parent_win_id
                                          , 0, 0, 100, 100
                                          , 0, WhitePixel(_display, 0)
                                          , BlackPixel(_display, 0));


    ACE_OS::fprintf(stdout, "create win (id = %lu) from parent(id = %lu)!\n"
                    , _current_win_id, _parent_win_id);

    ACE_OS::fflush(stdout);

    int attrListSgl[] =
    { GLX_RGBA, GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4, GLX_BLUE_SIZE, 4,
      GLX_DEPTH_SIZE, 16, None };
    int attrListDbl[] =
    { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 4, GLX_GREEN_SIZE, 4,
      GLX_BLUE_SIZE, 4, GLX_DEPTH_SIZE, 16, None };
    int screen = DefaultScreen(_display);
    XVisualInfo* visual = glXChooseVisual(_display, screen, attrListDbl);
    if (NULL == visual)
    {
        visual = glXChooseVisual(_display, screen, attrListSgl);
        fprintf(stdout, "Singlebuffered : true\n");
        if(NULL == visual)
        {
            fprintf(stdout, "Could not get suitable XVisualInfo\n");
            return -1;
        }
    }
    else
    {
        fprintf(stdout, "Doublebuffered : true\n");
    }
    // Create the rendering context
    _glcontext = glXCreateContext(_display, visual, 0, GL_TRUE);
    // Create a colormap
    Colormap colormap = XCreateColormap
                        (_display, RootWindow(_display, visual->screen),
                         visual->visual, AllocNone);
    // Make the rendering context current, perform initialization, then
    // deselect it
    if(False == glXMakeCurrent(_display, _current_win_id, _glcontext))
    {
        fprintf(stdout, "glXMakeCurrent failed!\n");
        return -1;
    }

    GLenum err = glewInit();
    if(err != 0)
    {
        GLubyte const *err_str = glewGetErrorString(err);
        fprintf(stderr, "%s\n", err_str);
        return -1;
    }

    _render = new OGL_YUV_Render();
//    OGL_YUV_Render render;

    if(!_render->init(false))
    {
        fprintf(stderr, "OGL_YUV_Render init error!\n");
        return -1;
    }

    XMapWindow(_display, _current_win_id);
    XFlush(_display);
    XSync(_display, true);

    _is_inited = true;

    _win_change = new XWindowChanges;
    _win_attr = new XWindowAttributes;

    glXMakeCurrent(_display, None, NULL);

    return 0;
}

int OGL_Widget_Impl::paint(OGL_WIDGET_IMPL::YUV_Data *yuv_data)
{
    ACE_Guard<ACE_Thread_Mutex> guard_mutex(_mutex);

    // Make the rendering context current, perform initialization, then
    // deselect it
    if(False == glXMakeCurrent(_display, _current_win_id, _glcontext))
    {
        fprintf(stdout, "glXMakeCurrent failed!\n");
        return -1;
    }


    if (_is_run_painter)
    {
        if(True == XGetWindowAttributes(_display, _parent_win_id, _win_attr))
        {
            if ((_old_width != _win_attr->width) ||
                    ( _old_height != _win_attr->height))
            {
                unsigned int vmask = CWWidth | CWHeight;
                _win_change->width = _win_attr->width;
                _win_change->height = _win_attr->height;
                XConfigureWindow(_display, _current_win_id, vmask, _win_change);
                glViewport(0, 0, _win_attr->width, _win_attr->height);
                _old_width = _win_attr->width;
                _old_height = _win_attr->height;
            }
        }

        if (NULL == yuv_data)
        {
            fprintf(stdout, "yuv_data NULL!!!!!\r\n");
            fflush(stdout);
            return -1;
        }

        (*_render)(yuv_data->width, yuv_data->height, yuv_data->buffer, NULL);
        glXSwapBuffers(_display, _current_win_id);
    }

    glXMakeCurrent(_display, None, NULL);

    delete yuv_data;
    yuv_data = NULL;
    return 0;
}

void* OGL_Widget_Impl::painter(void* arg)
{
    OGL_Widget_Impl *impl = reinterpret_cast<OGL_Widget_Impl*>(arg);
    impl->painter_func();
    return NULL;
}

int OGL_Widget_Impl::on_yuv_data_arrive(int w, int h, char ** yuv, int* linesize )
{
    //    fprintf(stdout, "Ent OGL_Widget_Impl::on_yuv_data_arrive() !!!!!!!!!!!!!!!!!!!!!!\r\n\r\n");
    //    fflush(stdout);
    if( !_is_inited )
        return 0;

    OGL_WIDGET_IMPL::YUV_Data *yuv_data = new OGL_WIDGET_IMPL::YUV_Data();
    if (NULL == yuv_data)
    {
        fprintf(stdout, "OGL_Widget_Impl::on_yuv_data_arrive() new fail\r\n");
        return -1;
    }

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

    // ACE_Time_Value tv(0, 500);
#ifdef  SINGLE_THREAD
    this->paint(yuv_data);
#else
    int rc = _yuv_queue.enqueue(yuv_data); // ?time out
    if (rc == -1)
    {
        fprintf(stdout, "_yuv_queue.enqueue error!!!!!\r\n");
        fflush(stdout);
    }
    //    fprintf(stdout, "lev OGL_Widget_Impl::on_yuv_data_arrive() !!!!!!!!!!!!!!!!!!!!!!\r\n\r\n");
    //    fflush(stdout);
#endif
    return 0;
}
