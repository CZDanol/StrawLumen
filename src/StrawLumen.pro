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
    rec/playlist.cpp \
    modelview/playlistitemmodel.cpp \
    gui/mainwindow_presentationmode.cpp \
    util/standarddialogs.cpp \
    util/execonmainthread.cpp \
    util/scopeexit.cpp \
    modelview/slidesitemmodel.cpp \
    modelview/slidesitemdelegate.cpp \
    presentation/powerpoint/presentation_powerpoint.cpp \
    presentation/presentationengine.cpp \
    presentation/powerpoint/presentationengine_powerpoint.cpp \
    presentation/presentationmanager.cpp \
    gui/activexdebugdialog.cpp \
    presentation/presentationpropertieswidget.cpp \
    presentation/powerpoint/presentationpropertieswidget_powerpoint.cpp \
    gui/projectorwindow.cpp \
    gui/settingsdialog.cpp \
    wgt/displayselectionwidget.cpp \
    presentation/native/presentationengine_native.cpp \
    presentation/native/presentation_blackscreen.cpp \
    presentation/native/presentation_nativepresentation.cpp \
    gui/mainwindow_songsmode.cpp \
    job/dbmanager.cpp \
    job/db.cpp \
    wgt/songlistwidget.cpp \
    job/settings.cpp \
    rec/chord.cpp \
    util/songcontentsyntaxhiglighter.cpp \
    wgt/songcontenttextedit.cpp \
    rec/songsection.cpp \
    wgt/extendedlineedit.cpp \
    wgt/wordcompletinglineedit.cpp \
    gui/backgrounddialog.cpp \
    gui/stylesdialog.cpp \
    wgt/colorselectionwidget.cpp \
    wgt/colorcomponentwidget.cpp \
    wgt/fontselectionwidget.cpp \
    wgt/textstylewidget.cpp \
    wgt/textstylepreviewwidget.cpp \
    rec/textstyle.cpp \
    rec/presentationstyle.cpp \
    wgt/presentationstylepreviewwidget.cpp \
    job/jsonautomation.cpp \
    job/fontdatabase.cpp \
    rec/presentationbackground.cpp \
    wgt/presentationbackgroundpreviewwidget.cpp \
    job/backgroundmanager.cpp \
    wgt/presentationbackgroundselectionwidget.cpp \
    presentation/native/presentation_song.cpp \
    wgt/styleselectionwidget.cpp \
    presentation/native/presentationpropertieswidget_song.cpp

FORMS += \
    gui/mainwindow.ui \
    gui/splashscreen.ui \
    gui/mainwindow_presentationmode.ui \
    gui/activexdebugdialog.ui \
    presentation/presentationpropertieswidget.ui \
    presentation/powerpoint/presentationpropertieswidget_powerpoint.ui \
    gui/projectorwindow.ui \
    gui/settingsdialog.ui \
    wgt/displayselectionwidget.ui \
    gui/mainwindow_songsmode.ui \
    wgt/songlistwidget.ui \
    gui/backgrounddialog.ui \
    gui/stylesdialog.ui \
    wgt/colorselectionwidget.ui \
    wgt/fontselectionwidget.ui \
    wgt/textstylewidget.ui \
    wgt/presentationbackgroundselectionwidget.ui \
    wgt/styleselectionwidget.ui \
    presentation/native/presentationpropertieswidget_song.ui

HEADERS += \
    gui/mainwindow.h \
    presentation/presentation.h \
    util/jobthread.h \
    job/activexjobthread.h \
    gui/splashscreen.h \
    rec/playlist.h \
    modelview/playlistitemmodel.h \
    gui/mainwindow_presentationmode.h \
    util/standarddialogs.h \
    util/execonmainthread.h \
    util/scopeexit.h \
    modelview/slidesitemmodel.h \
    modelview/slidesitemdelegate.h \
    presentation/powerpoint/presentation_powerpoint.h \
    presentation/presentationengine.h \
    presentation/powerpoint/presentationengine_powerpoint.h \
    presentation/presentationmanager.h \
    gui/activexdebugdialog.h \
    presentation/presentationpropertieswidget.h \
    presentation/powerpoint/presentationpropertieswidget_powerpoint.h \
    gui/projectorwindow.h \
    gui/settingsdialog.h \
    wgt/displayselectionwidget.h \
    presentation/native/presentationengine_native.h \
    presentation/native/presentation_blackscreen.h \
    presentation/native/presentation_nativepresentation.h \
    gui/mainwindow_songsmode.h \
    job/dbmanager.h \
    job/db.h \
    job/dbmigration.h \
    wgt/songlistwidget.h \
    job/settings.h \
    rec/chord.h \
    util/songcontentsyntaxhiglighter.h \
    wgt/songcontenttextedit.h \
    rec/songsection.h \
    wgt/extendedlineedit.h \
    wgt/wordcompletinglineedit.h \
    gui/backgrounddialog.h \
    main.h \
    gui/stylesdialog.h \
    wgt/colorselectionwidget.h \
    wgt/colorcomponentwidget.h \
    wgt/fontselectionwidget.h \
    wgt/textstylewidget.h \
    wgt/textstylepreviewwidget.h \
    rec/textstyle.h \
    rec/presentationstyle.h \
    wgt/presentationstylepreviewwidget.h \
    job/jsonautomation.h \
    job/fontdatabase.h \
    rec/presentationbackground.h \
    wgt/presentationbackgroundpreviewwidget.h \
    job/backgroundmanager.h \
    wgt/presentationbackgroundselectionwidget.h \
    presentation/native/presentation_song.h \
    wgt/styleselectionwidget.h \
    presentation/native/presentationpropertieswidget_song.h

RESOURCES += \
    ../res/resources.qrc

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

Debug:LIBS += -L$$PWD/../lib/debug/
Release:LIBS += -L$$PWD/../lib/release/

Release:DESTDIR = ../bin/bin_x86
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
