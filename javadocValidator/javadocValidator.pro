TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    tokenizer.cpp \
    parser.cpp

HEADERS += \
    tokenizer.h \
    parser.h

OTHER_FILES +=  \
    ../input.c
