#-------------------------------------------------
#
# Project created by QtCreator 2018-12-31T17:36:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LabelingTool
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    custompoint.cpp \
    graphics_view_zoom.cpp \
    customline.cpp \
    util.cpp \
    filemgr.cpp \
    jsonparser.cpp

HEADERS  += mainwindow.h \
    custompoint.h \
    graphics_view_zoom.h \
    customline.h \
    util.h \
    filemgr.h \
    jsonparser.h

FORMS    += mainwindow.ui
