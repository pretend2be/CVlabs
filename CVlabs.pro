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
    filters.cpp \
    poi.cpp \
    qimagemaker.cpp \
    descriptors.cpp

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    matrix.h \
    image.h \
    kernels.h \
    pyramid.h \
    filters.h \
    poi.h \
    qimagemaker.h \
    descriptors.h
