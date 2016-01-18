#-------------------------------------------------
#
# Project created by QtCreator 2016-01-08T14:24:40
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = ../lib/ep_protocol
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH += $(TOOLKITS)/include

SOURCES += \
    rtp_recv_handler.cpp \
    udp_socket.cpp \
    rtp2nalu_h264.cpp \
    nalu2rtp_handler.cpp \
    nalu2rtp_h264.cpp

HEADERS += \
    rtp_recv_handler.h \
    protocol_defines.h \
    udp_socket.h \
    protocol_types.h \
    rtp2nalu_h264.h \
    nalu2rtp_handler.h \
    nalu2rtp_h264.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
