#-------------------------------------------------
#
# Project created by QtCreator 2013-04-06T21:09:53
#
#-------------------------------------------------

QT      += winextras

TARGET = ListPrograms
TEMPLATE = lib
CONFIG += staticlib

SOURCES += program.cpp \
           $$PWD/../../C++/ListPrograms/InstalledPrograms.cpp \
           $$PWD/../../C++/ListPrograms/ListPrograms.cpp \
           $$PWD/../../C++/ListPrograms/RegistryKey.cpp \
           $$PWD/../../C++/ListPrograms/Software.cpp \
           $$PWD/../../C++/ListPrograms/Util.cpp

HEADERS += program.h \
           $$PWD/../../C++/ListPrograms/Architecture.h \
           $$PWD/../../C++/ListPrograms/framework.h \
           $$PWD/../../C++/ListPrograms/InstalledPrograms.h \
           $$PWD/../../C++/ListPrograms/ListPrograms.h \
           $$PWD/../../C++/ListPrograms/RegistryKey.h \
           $$PWD/../../C++/ListPrograms/Software.h \
           $$PWD/../../C++/ListPrograms/Util.h

INCLUDEPATH += $$PWD/../../C++/ListPrograms

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
