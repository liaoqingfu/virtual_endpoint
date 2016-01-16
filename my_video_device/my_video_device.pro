#-------------------------------------------------
#
# Project created by QtCreator 2013-12-17T14:52:02
#
#-------------------------------------------------

QT       -= gui

TARGET = ../lib/my_video_device
TEMPLATE = lib
#CONFIG += staticlib
VERSION = 1.0.2

DEFINES += MY_DEVICE_LIBRARY

INCLUDEPATH += ..
INCLUDEPATH += ../../
INCLUDEPATH += $(TOOLKITS)/include
INCLUDEPATH += $(TOOLKITS)/include/v4l

#LIBS += -L$(TOOLKITS)/lib -lv4l2 -lv4lconvert
#LIBS += -L../lib -lvideo_capture

SOURCES += my_video_device.cpp

HEADERS += my_video_device.h
