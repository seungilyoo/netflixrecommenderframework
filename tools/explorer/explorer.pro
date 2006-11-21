TEMPLATE = app
TARGET = 
DESTDIR = ../../bin
DEPENDPATH += .
INCLUDEPATH += .
#CONFIG += debug

# Input

include(../../src/netflixresultsgenerator.pri)

FORMS += mainwindow.ui
SOURCES += main.cpp
