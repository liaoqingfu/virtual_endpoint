QT       -= core gui

QMAKE_CXXFLAGS = -std=c++0x

TARGET = ../lib/myv4l2test
TEMPLATE = lib
#CONFIG += staticlib
VERSION = 1.0.2

INCLUDEPATH += $(TOOLKITS)/include /home/wisdom/pingsan/my_video_device/

LIBS += -L$(TOOLKITS)/lib -lv4l2 -L/home/wisdom/pingsan/lib -lmy_video_device
DEFINES += MY_V4L2_LIBRARY

SOURCES += \
    my_video_device_test.cpp

HEADERS += \
        my_video_device_test.h

