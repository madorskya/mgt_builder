TEMPLATE = app
CONFIG += console 
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH = /opt/cactus/include
INCLUDEPATH += ../../../linenoise_cpp/
DEFINES += MTF7
QMAKE_CXXFLAGS += -std=c++11 -O2

SOURCES += main.cpp \
    fpga.cpp
SOURCES += drp_unit.cpp
SOURCES += ../../../linenoise_cpp/linenoise.cpp
HEADERS += drp_unit.h \
    fpga.h
HEADERS += ../../../linenoise_cpp/linenoise.h


LIBS += -L/opt/cactus/lib/ -lreadline -lncurses -lboost_regex -lboost_filesystem -lboost_system -lboost_serialization

DEPENDPATH += /opt/cactus/include
