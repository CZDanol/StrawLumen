#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T10:11:40
#
#-------------------------------------------------

QT       += core gui printsupport widgets axcontainer

# For debugging purposes
# CONFIG += console

TEMPLATE = app

VERSION = 0.0.1.0
QMAKE_TARGET_COMPANY = "Straw Solutions"
QMAKE_TARGET_PRODUCT = "Straw Lumen"
QMAKE_TARGET_COPYRIGHT = "(c) 2018 Straw Solutions"

DEFINES += PROGRAM_VERSION=\\\"$$VERSION\\\"
win32:DEFINES += PLATFORM_SUFFIX=\\\"win32\\\"

TARGET = strawLumen

CONFIG += c++17

# Without this, the program can use max 2GB ram
QMAKE_LFLAGS = /LARGEADDRESSAWARE

win32:RC_ICONS += ../res/icon.ico

SOURCES += main.cpp \
    gui/mainwindow.cpp \
    presentation/presentation.cpp \
    presentation/powerpointpresentation.cpp

FORMS += \
    gui/mainwindow.ui

HEADERS += \
    gui/mainwindow.h \
    presentation/presentation.h \
    presentation/powerpointpresentation.h

RESOURCES += \
    ../res/resources.qrc

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

Debug:LIBS += -L$$PWD/../lib/debug/
Release:LIBS += -L$$PWD/../lib/release/

Release:DESTDIR =/../bin/bin_x86
Debug:DESTDIR = ../bin/bin_x86_debug

Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui
Release:PRECOMPILED_DIR = release

Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
Debug:PRECOMPILED_DIR = debug
