QT += core
QT += gui

CONFIG += c++14

TARGET = CVlabs
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    matrix.cpp \
    image.cpp \
    pyramid.cpp \
    pyramidlvl.cpp \
    pyramidoctave.cpp \
    filters.cpp

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    matrix.h \
    image.h \
    kernels.h \
    pyramid.h \
    pyramidlvl.h \
    pyramidoctave.h \
    filters.h
