

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS



SOURCES += main.cpp\
        mainwindow.cpp \
    workarea.cpp \
    imagecontainer.cpp \
    colorboard.cpp \
    colorlabel.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    workarea.h \
    imagecontainer.h \
    colorboard.h \
    colorlabel.h \
    util.h
