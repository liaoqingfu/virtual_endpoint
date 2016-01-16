#-------------------------------------------------
#
# Project created by QtCreator 2013-12-19T16:49:40
#
#-------------------------------------------------

QT       += widgets opengl core

TARGET = ../lib/video_render
TEMPLATE = lib

#TARGET = app
#CONFIG += staticlib
CONFIG += debug
VERSION = 1.0.2

QMAKE_CXXFLAGS += -std=c++0x

DEFINES += VIDEO_RENDER_LIBRARY

INCLUDEPATH += ..
INCLUDEPATH += ../../
INCLUDEPATH += ../ffmpeg_lib/ffmpeg ../ffmpeg_lib
INCLUDEPATH += $(TOOLKITS)/include
INCLUDEPATH += $(TOOLKITS)/include/v4l
INCLUDEPATH += ../conf_ctrl

#LIBS += -L../lib -lffmpeg_lib
#LIBS += -L$(TOOLKITS)/lib -lACE
#LIBS += -lGLEW -lGLU -lglut

SOURCES += \
    yuv_widget.cpp \
    yuv2rgb.cpp \
    yuv_widget_impl.cpp \
    yuv_gl_widget.cpp \
    glInfo.cpp \
    main.cpp \
    yuv_gl_widget_impl.cpp \
    my_msg_block_pool.cpp \
    video_render.cpp \
    ogl_widget.cpp \
    ogl_widget_impl.cpp \
    ogl_yuv_render.cpp

HEADERS += \
    yuv_widget.h \
    yuv2rgb.h \
    yuv_widget_impl.h \
    yuv_gl_widget.h \
    glInfo.h \
    yuv_shader_src.h \
    yuv_gl_widget_impl.h \
    my_msg_block_pool.h \
    video_render.h \
    ogl_widget.h \
    ogl_widget_impl.h \
    ogl_yuv_render.h \
    yuv_event_listener.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
