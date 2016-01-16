QT -= core
QT -= gui

TARGET = video_encoder_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = ../../bin/video_encoder_test

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += ../../
INCLUDEPATH += $(TOOLKITS)/include
INCLUDEPATH += ../../video_encoder

LIBS += -L../../lib -lvideo_capture  -lvideo_render -lmy_v4l2 -lffmpeg_lib
LIBS += -L../lib -lmy_video_device
LIBS += -L../../lib -lvideo_encoder

LIBS += -L$(TOOLKITS)/lib  -lACE -lv4l2 -lv4lconvert -lx264 -lx265

SOURCES += main.cpp

