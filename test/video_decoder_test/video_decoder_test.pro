TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = ../../bin/video_decoder_test

QMAKE_CXXFLAGS += -std=c++0x


INCLUDEPATH += $(TOOLKITS)/include
INCLUDEPATH += ../../video_decoder
INCLUDEPATH += ../../ep_protocol

LIBS += -L../../lib -lvideo_decoder -lep_protocol
LIBS += -L$(TOOLKITS)/lib -lavformat  -lavcodec -lswscale -lavutil  -lswresample -lACE
LIBS += -lpthread

SOURCES += main.cpp
HEADERS += \
    rtp.h

