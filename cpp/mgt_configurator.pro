TEMPLATE = app
CONFIG += console 
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    fpga.cpp
SOURCES += drp_unit.cpp
HEADERS += drp_unit.h \
    fpga.h

