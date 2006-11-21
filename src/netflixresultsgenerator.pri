INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
SOURCES += $$PWD/database.cpp $$PWD/movie.cpp $$PWD/probe.cpp $$PWD/user.cpp
HEADERS += $$PWD/database.h $$PWD/movie.h $$probe.h $$PWD/rmse.h $$PWD/user.h

win32 {
    SOURCES += $$PWD/winmmap.cpp
    HEADERS += $$PWD/winmmap.h
}

