QT += core
QT -= gui

CONFIG += c++11

TARGET = reg_configurator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    drp_unit.cpp

HEADERS += \
    drp_unit.h
