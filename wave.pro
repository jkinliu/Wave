#-------------------------------------------------
#
# Project created by QtCreator 2015-09-02T19:47:22
#
#-------------------------------------------------

QT       += core gui network serialport ftp

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wave
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    plot.cpp \
    ftp.cpp \
    protocol.cpp \
    sacformat.cpp \
    tcpclient.cpp \
    fileCtrl.cpp \
    modify.cpp \
    setting.cpp

HEADERS  += widget.h \
    common.h \
    plot.h \
    ftp.h \
    protocol.h \
    sacformat.h \
    tcpclient.h \
    global.h \
    fileCtrl.h \
    modify.h \
    setting.h

FORMS    += widget.ui \
    modify.ui \
    setting.ui

win32: include (C:/Qwt-6.1.2/features/qwt.prf)
unix:  include (/usr/local/qwt-6.2.0-svn/features/qwt.prf)

DISTFILES += \
    wave.pro.user

win32: LIBS += -lws2_32

RC_ICONS = images/logo.ico

