#-------------------------------------------------
#
# Project created by QtCreator 2015-04-09T16:43:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++0x

TARGET = ../../bin/video_capture_render
TEMPLATE = app

INCLUDEPATH += ../../
INCLUDEPATH += $(TOOLKITS)/include

LIBS += -L../../lib -lvideo_capture  -lvideo_render -lmy_v4l2 -lffmpeg_lib
LIBS += -L../lib -lmy_video_device
LIBS += -L$(TOOLKITS)/lib -lswscale -lavutil -lavcodec
LIBS += -L$(TOOLKITS)/lib  -lACE -lv4l2 -lv4lconvert

LIBS += -L$(TOOLKITS)/lib -lGLEW -lGLU


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
