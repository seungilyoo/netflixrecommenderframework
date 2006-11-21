TEMPLATE = app
TARGET = 
DESTDIR = ../../bin
DEPENDPATH += .
INCLUDEPATH += .
QT = core
CONFIG += debug
# Input

include(../../src/netflixresultsgenerator.pri)

SOURCES += main.cpp
