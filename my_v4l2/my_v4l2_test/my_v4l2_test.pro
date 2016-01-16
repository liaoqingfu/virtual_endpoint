QT       -= core gui

QMAKE_CXXFLAGS = -std=c++0x

TARGET = ../lib/myv4l2test
TEMPLATE = lib
#CONFIG += staticlib
VERSION = 1.0.2

INCLUDEPATH += $(TOOLKITS)/include /home/wisdom/pingsan/my_v4l2/

LIBS += -L$(TOOLKITS)/lib -lv4l2 -L/home/wisdom/pingsan/lib -lmy_v4l2
DEFINES += MY_V4L2_LIBRARY

SOURCES += \
    my_v4l2_test.cpp

HEADERS += \
	my_v4l2_test.h
