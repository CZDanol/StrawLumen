#-------------------------------------------------
#
# Project created by QtCreator 2016-02-15T10:11:40
#
#-------------------------------------------------

QT       += core gui printsupport multimedia multimediawidgets widgets axcontainer sql xml webenginewidgets network webengine

# For debugging purposes
# CONFIG += console

TEMPLATE = app

VERSION = 1.3.7.0
QMAKE_TARGET_COMPANY = "Danol"
QMAKE_TARGET_PRODUCT = "Straw Lumen"
QMAKE_TARGET_COPYRIGHT = "(c) 2022 Danol"
QMAKE_TARGET_DESCRIPTION = "Straw Lumen"

win32|win64 {
	RC_ICONS += $$PWD/../res/logos/lumen_icon.ico
	OS_STR = win
}

DEFINES += PRODUCT_IDSTR=\\\"lumen\\\"
DEFINES += PROGRAM_VERSION=\\\"$$VERSION\\\"
DEFINES += PROGRAM_UPSTREAM_VERSION=\\\"1.3.1.0\\\" # If the updater gets this upstream version, it does not promote update
DEFINES += PLATFORM_ID=\\\"$${OS_STR}_$${QMAKE_TARGET.arch}\\\"

TRANSLATIONS = ../bin/lang/en.ts ../bin/lang/slo.ts

TARGET = strawLumen

CONFIG += c++17

*msvc* {
	QMAKE_CXXFLAGS += -MP # Parallel build
}

# Statically link MSVC distributables
# Release:CONFIG += static_runtime TODO

# Without this, the program can use max 2GB ram
QMAKE_LFLAGS = /LARGEADDRESSAWARE

Debug:RELEASE_STR = _dbg
Release:RELEASE_STR =

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

LIBS += -L$$PWD/../lib/$${OS_STR}_$${QMAKE_TARGET.arch}$${RELEASE_STR}/
LIBS += -L$$PWD/../lib/$${OS_STR}_$${QMAKE_TARGET.arch}_common/
DESTDIR = $$PWD/../bin/bin_$${OS_STR}_$${QMAKE_TARGET.arch}$${RELEASE_STR}

DISTFILES += \
		../res/stylesheet_dark.css

SOURCES += main.cpp \
		gui/bibletranslationmgmtdialog.cpp \
		gui/mainwindow.cpp \
		gui/quickbibleversewindow.cpp \
		gui/songsmodewindow.cpp \
		importexport/powerpoint/powerpointimportdialog.cpp \
		presentation/presentation.cpp \
		strawapi/strawapi.cpp \
		strawapi/updatemanager.cpp \
		util/biblerefsyntaxhighlighter.cpp \
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
		gui/settingsdialog.cpp \
		util/updatesdisabler.cpp \
		wgt/bibletranslationlistbox.cpp \
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
		presentation/native/presentationpropertieswidget_song.cpp \
		modelview/songsitemmodel.cpp \
		importexport/documentgenerationdialog.cpp \
		gui/aboutdialog.cpp \
		wgt/multisongselectionwidget.cpp \
		modelview/songrecorditemmodel.cpp \
		wgt/extendedtreeview.cpp \
		gui/startupsplashscreen.cpp \
		importexport/lumenexportdialog.cpp \
		importexport/exportdb.cpp \
		importexport/lumenimportdialog.cpp \
		wgt/songtagslineedit.cpp \
		importexport/opensongimportdialog.cpp \
		util/guianimations.cpp \
		importexport/opensongexportdialog.cpp \
		gui/playlistsdialog.cpp \
		util/fusionproxystyle.cpp \
		job/widgetvalues.cpp \
		presentation/native/presentation_customslide.cpp \
		presentation/native/presentationpropertieswidget_customslide.cpp \
		strawapi/feedbackdialog.cpp \
		job/dbmigration.cpp \
		job/wordsplit.cpp \
		util/regex.cpp \
		util/macroutils.cpp \
		gui/bulkeditsongsdialog.cpp \
		presentation/native/presentation_images.cpp \
		presentation/native/presentationpropertieswidget_images.cpp \
		job/asynccachemanager.cpp \
		presentation/native/nativeprojectorwindow.cpp \
		presentation/video/videoprojectorwindow.cpp \
		presentation/video/presentationengine_video.cpp \
		presentation/video/presentation_video.cpp \
		presentation/video/presentationpropertieswidget_video.cpp \
		rec/biblebook.cpp \
		job/parsebible.cpp \
		presentation/native/presentation_bibleverse.cpp \
		presentation/native/presentationpropertieswidget_bibleverse.cpp \
		wgt/bibleverseselectionwidget.cpp \
		presentation/native/presentation_bibleverse_wizard.cpp \
		rec/bibleref.cpp \
		wgt/verselistplaintextedit.cpp \
		wgt/videocontroltabwidget.cpp \
		presentation/web/presentation_web.cpp \
		presentation/web/presentationengine_web.cpp \
		presentation/web/webprojectorwindow.cpp \
		presentation/web/presentationpropertieswidget_web.cpp

FORMS += \
		gui/bibletranslationmgmtdialog.ui \
		gui/mainwindow.ui \
		gui/quickbibleversewindow.ui \
		gui/songsmodewindow.ui \
		gui/splashscreen.ui \
		gui/mainwindow_presentationmode.ui \
		gui/activexdebugdialog.ui \
		importexport/powerpoint/powerpointimportdialog.ui \
		presentation/presentationpropertieswidget.ui \
		presentation/powerpoint/presentationpropertieswidget_powerpoint.ui \
		presentation/native/nativeprojectorwindow.ui \
		gui/settingsdialog.ui \
		strawapi/updatemanager.ui \
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
		presentation/native/presentationpropertieswidget_song.ui \
		importexport/documentgenerationdialog.ui \
		gui/aboutdialog.ui \
		wgt/multisongselectionwidget.ui \
		gui/startupsplashscreen.ui \
		importexport/lumenexportdialog.ui \
		importexport/lumenimportdialog.ui \
		importexport/opensongimportdialog.ui \
		importexport/opensongexportdialog.ui \
		gui/playlistsdialog.ui \
		presentation/native/presentationpropertieswidget_customslide.ui \
		strawapi/feedbackdialog.ui \
		gui/bulkeditsongsdialog.ui \
		presentation/native/presentationpropertieswidget_images.ui \
		presentation/video/videoprojectorwindow.ui \
		presentation/video/presentationpropertieswidget_video.ui \
		presentation/native/presentationpropertieswidget_bibleverse.ui \
		wgt/bibleverseselectionwidget.ui \
		presentation/native/presentation_bibleverse_wizard.ui \
		wgt/videocontroltabwidget.ui \
		presentation/web/webprojectorwindow.ui \
		presentation/web/presentationpropertieswidget_web.ui

HEADERS += \
		gui/bibletranslationmgmtdialog.h \
		gui/mainwindow.h \
		gui/quickbibleversewindow.h \
		gui/songsmodewindow.h \
		importexport/powerpoint/powerpointimportdialog.h \
		presentation/presentation.h \
		strawapi/strawapi.h \
		strawapi/updatemanager.h \
		util/biblerefsyntaxhighlighter.h \
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
		gui/settingsdialog.h \
		util/updatesdisabler.h \
		wgt/bibletranslationlistbox.h \
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
		presentation/native/presentationpropertieswidget_song.h \
		modelview/songsitemmodel.h \
		importexport/documentgenerationdialog.h \
		gui/aboutdialog.h \
		wgt/multisongselectionwidget.h \
		modelview/songrecorditemmodel.h \
		wgt/extendedtreeview.h \
		gui/startupsplashscreen.h \
		importexport/lumenexportdialog.h \
		importexport/exportdb.h \
		importexport/lumenimportdialog.h \
		wgt/songtagslineedit.h \
		importexport/opensongimportdialog.h \
		util/guianimations.h \
		importexport/opensongexportdialog.h \
		gui/playlistsdialog.h \
		util/fusionproxystyle.h \
		job/widgetvalues.h \
		presentation/native/presentation_customslide.h \
		presentation/native/presentationpropertieswidget_customslide.h \
		strawapi/feedbackdialog.h \
		job/wordsplit.h \
		util/regex.h \
		util/macroutils.h \
		gui/bulkeditsongsdialog.h \
		presentation/native/presentation_images.h \
		presentation/native/presentationpropertieswidget_images.h \
		job/asynccachemanager.h \
		presentation/native/nativeprojectorwindow.h \
		presentation/video/videoprojectorwindow.h \
		presentation/video/presentationengine_video.h \
		presentation/video/presentation_video.h \
		presentation/video/presentationpropertieswidget_video.h \
		rec/biblebook.h \
		job/parsebible.h \
		presentation/native/presentation_bibleverse.h \
		presentation/native/presentationpropertieswidget_bibleverse.h \
		wgt/bibleverseselectionwidget.h \
		presentation/native/presentation_bibleverse_wizard.h \
		rec/bibleref.h \
		wgt/verselistplaintextedit.h \
		wgt/videocontroltabwidget.h \
		presentation/web/presentation_web.h \
		presentation/web/presentationengine_web.h \
		presentation/web/webprojectorwindow.h \
		presentation/web/presentationpropertieswidget_web.h

RESOURCES += \
		../res/resources.qrc
