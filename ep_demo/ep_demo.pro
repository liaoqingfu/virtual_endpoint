#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T16:58:40
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ../bin/ep_demo
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x


INCLUDEPATH += ../
INCLUDEPATH += $(TOOLKITS)/include

INCLUDEPATH += ../ep_interface


LIBS += -L../lib -lep_interface -lep_protocol -lvideo_encoder -lvideo_decoder -lvideo_render
LIBS += -L../../lib -lvideo_capture  -lvideo_render -lmy_v4l2 -lffmpeg_lib
LIBS += -L../lib -lmy_video_device

LIBS += -L$(TOOLKITS)/lib -lswscale -lavutil -lavcodec -lswresample
LIBS += -L$(TOOLKITS)/lib  -lACE -lv4l2 -lv4lconvert

LIBS += -L$(TOOLKITS)/lib -lGLEW -lGLU -lx264 -lx265

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
