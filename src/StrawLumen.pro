#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T10:11:40
#
#-------------------------------------------------

QT       += core gui printsupport widgets axcontainer sql

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
    util/jobthread.cpp \
    job/activexjobthread.cpp \
    gui/splashscreen.cpp \
    presentation/playlist.cpp \
    util/playlistitemmodel.cpp \
    gui/mainwindow_presentationmode.cpp \
    util/standarddialogs.cpp \
    util/execonmainthread.cpp \
    util/scopeexit.cpp \
    util/slidesitemmodel.cpp \
    util/slidesitemdelegate.cpp \
    presentation/presentation_powerpoint.cpp \
    presentation/presentationengine.cpp \
    presentation/presentationengine_powerpoint.cpp \
    presentation/presentationmanager.cpp \
    gui/activexdebugdialog.cpp \
    gui/presentationpropertieswidget.cpp \
    gui/presentationpropertieswidget_powerpoint.cpp \
    gui/projectorwindow.cpp \
    gui/settingsdialog.cpp \
    gui/displayselectionwidget.cpp \
    presentation/presentationengine_native.cpp \
    presentation/presentation_blackscreen.cpp \
    presentation/presentation_nativepresentation.cpp \
    gui/mainwindow_songsmode.cpp \
    job/dbmanager.cpp \
    job/db.cpp \
    gui/songlistwidget.cpp \
    job/settings.cpp \
    util/chord.cpp \
    util/songcontentsyntaxhiglighter.cpp \
    util/songcontenttextedit.cpp \
    util/songsection.cpp \
    util/extendedlineedit.cpp \
    util/wordcompletinglineedit.cpp

FORMS += \
    gui/mainwindow.ui \
    gui/splashscreen.ui \
    gui/mainwindow_presentationmode.ui \
    gui/activexdebugdialog.ui \
    gui/presentationpropertieswidget.ui \
    gui/presentationpropertieswidget_powerpoint.ui \
    gui/projectorwindow.ui \
    gui/settingsdialog.ui \
    gui/displayselectionwidget.ui \
    gui/mainwindow_songsmode.ui \
    gui/songlistwidget.ui

HEADERS += \
    gui/mainwindow.h \
    presentation/presentation.h \
    util/jobthread.h \
    job/activexjobthread.h \
    gui/splashscreen.h \
    presentation/playlist.h \
    util/playlistitemmodel.h \
    gui/mainwindow_presentationmode.h \
    util/standarddialogs.h \
    util/execonmainthread.h \
    util/scopeexit.h \
    util/slidesitemmodel.h \
    util/slidesitemdelegate.h \
    presentation/presentation_powerpoint.h \
    presentation/presentationengine.h \
    presentation/presentationengine_powerpoint.h \
    presentation/presentationmanager.h \
    gui/activexdebugdialog.h \
    gui/presentationpropertieswidget.h \
    gui/presentationpropertieswidget_powerpoint.h \
    gui/projectorwindow.h \
    gui/settingsdialog.h \
    gui/displayselectionwidget.h \
    presentation/presentationengine_native.h \
    presentation/presentation_blackscreen.h \
    presentation/presentation_nativepresentation.h \
    gui/mainwindow_songsmode.h \
    job/dbmanager.h \
    job/db.h \
    job/dbmigration.h \
    gui/songlistwidget.h \
    job/settings.h \
    util/chord.h \
    util/songcontentsyntaxhiglighter.h \
    util/songcontenttextedit.h \
    util/songsection.h \
    util/extendedlineedit.h \
    util/wordcompletinglineedit.h

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
