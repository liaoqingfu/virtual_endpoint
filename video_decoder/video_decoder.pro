TEMPLATE = lib
TARGET = ../lib/video_decoder

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#VERSION = 4.0.0

QMAKE_CXXFLAGS += -std=c++0x
#QMAKE_LFLAGS+=-rdynamic

INCLUDEPATH += $(TOOLKITS)/include

LIBS += -L$(TOOLKITS)/lib -lavformat  -lavcodec -lswscale -lavutil -lswresample

SOURCES += \
    video_decoder_factory.cpp \
    get_slice_type.cpp \
    video_decoder_ffmpeg.cpp
#    video_decoder_v3.cpp

HEADERS += \
    video_decoder.h \
    video_decoder_factory.h \
    get_slice_type.h \
    video_decoder_ffmpeg.h
#    video_decoder_v3.h

#DISTFILES += \
#    CMakeLists.txt

