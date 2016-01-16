#-------------------------------------------------
#
# Project created by QtCreator 2013-12-16T17:55:37
#
#-------------------------------------------------

TARGET = ../lib/ffmpeg_lib
TEMPLATE = lib
#VERSION = 1.0.2
#CONFIG += staticlib

INCLUDEPATH += ..
INCLUDEPATH += ../../
INCLUDEPATH += $(TOOLKITS)/include

#LIBS += -L$(TOOLKITS)/lib -lswscale -lavcodec -lavutil #-lavformat -lavdevice -lswresample -lavfilter
#LIBS += -L../lib -lswscale -lavcodec -lavutil
#LIBS += -L../lib -lswscale -lavutil -lavdevice -lavfilter -lswresample -lavformat -lavcodec

SOURCES += \
    ffmpeg_lib.cpp

HEADERS += \
    ffmpeg_lib.h

