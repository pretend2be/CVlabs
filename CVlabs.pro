QT += core
QT += gui

CONFIG += c++14

TARGET = CVlabs
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    matrix.cpp \
    image.cpp

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    matrix.h \
    image.h
