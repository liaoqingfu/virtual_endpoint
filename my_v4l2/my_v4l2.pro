#-------------------------------------------------
#
# Project created by QtCreator 2013-08-28T11:21:18
#
#-------------------------------------------------

QT       -= core gui

TARGET = ../lib/my_v4l2
TEMPLATE = lib
#CONFIG += staticlib
VERSION = 1.0.2

INCLUDEPATH += $(TOOLKITS)/include
INCLUDEPATH += $(TOOLKITS)/include/v4l

#LIBS += -L$(TOOLKITS)/lib -lv4l2
DEFINES += MY_V4L2_LIBRARY

SOURCES += \
    v4l2device.cpp

HEADERS += \
    v4l2device.h


