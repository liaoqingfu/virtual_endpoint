TEMPLATE = lib
TARGET = ../lib/video_encoder

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x
#QMAKE_LFLAGS+=-rdynamic

#VERSION = 4.0.0

INCLUDEPATH += $(TOOLKITS)/include

#LIBS += -L$(TOOLKITS)/lib -lx264 -lavcodec

SOURCES += \
    video_encoder_factory.cpp \
#    x264_lib_v4.cpp \
#    video_encoder_ffmpeg.cpp \
    video_encoder_h264.cpp \
    video_encoder_h265.cpp

HEADERS += \
    video_encoder.h \
    video_encoder_factory.h \
#    x264_lib_v4.h \
#    video_encoder_ffmpeg.h \
    encoder_types.h \
    video_encoder_h264.h \
    video_encoder_h265.h

