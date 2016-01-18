#-------------------------------------------------
#
# Project created by QtCreator 2016-01-12T10:20:25
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = ../lib/ep_interface
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++0x
INCLUDEPATH += $(TOOLKITS)/include
INCLUDEPATH += ../video_render  ../video_decoder ../ep_protocol ../video_capture
INCLUDEPATH += ../video_encoder

SOURCES += ep_interface.cpp \
    video_render_task.cpp \
    video_decoder_task.cpp \
    video_capture_task.cpp \
    video_encoder_task.cpp

HEADERS += ep_interface.h \
    ep_types.h \
    video_render_task.h \
    video_decoder_task.h \
    video_capture_task.h \
    video_encoder_task.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
