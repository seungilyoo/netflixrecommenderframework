QT = core
CONFIG += qtestlib
#CONFIG += debug

include(../src/netflixresultsgenerator.pri)

SOURCES		+= autotests.cpp
TARGET		= autotests
