#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "job/widgetvalues.h"

// F(settingName, uiControl)
#define SETTINGS_FACTORY(F)\
	F(display, dsDisplay)\
	F(song_defaultStyle, wgtDefaultPresentationStyle) F(song_emptySlideBefore, cbEmptySlideBeforeSong) F(song_emptySlideAfter, cbEmptySlideAfterSong)\
	F(ppt_blackSlideBefore, cbBlackSlideBeforePPT) F(ppt_blackSlideAfter, cbBlackSlideAfterPPT)

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

	friend class SettingsManager;

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

private slots:
	void onDisplayChanged(QScreen *current);

private slots:
	void on_btnClose_clicked();

private:
	Ui::SettingsDialog *ui;

};

extern SettingsDialog *settingsDialog;

#endif // SETTINGSDIALOG_H
