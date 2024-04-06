#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "job/widgetvalues.h"

// settingName, uiControl
#define SETTINGS_FACTORY(F)                              \
	F(darkMode, cbDarkMode)                                \
	F(defaultBibleTranslation, lstDefaultBibleTranslation) \
	F(language, cmbLanguage)                               \
                                                         \
	F(display, dsDisplay)                                  \
                                                         \
	F(song_defaultStyle, wgtDefaultPresentationStyle)      \
	F(song_emptySlideBefore, cbEmptySlideBeforeSong)       \
	F(song_emptySlideAfter, cbEmptySlideAfterSong)         \
	F(song_ignoreEmptySlides, cbIgnoreEmptySongSlides)     \
                                                         \
	F(ppt_blackSlideBefore, cbBlackSlideBeforePPT)         \
	F(ppt_blackSlideAfter, cbBlackSlideAfterPPT)

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog {
	Q_OBJECT

	friend class SettingsManager;

public:
	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

private slots:
	void updateBibleTranslationList();
	void updateLanguageList();

private slots:
	void onDisplayChanged(QScreen *current);

private slots:
	void on_btnClose_clicked();
	void on_cbDarkMode_clicked(bool checked);

private:
	Ui::SettingsDialog *ui;
};

extern SettingsDialog *settingsDialog;

#endif// SETTINGSDIALOG_H
