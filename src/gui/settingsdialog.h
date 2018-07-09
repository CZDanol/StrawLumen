#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

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
	void fillData();

private slots:
	void onDisplayChanged(QScreen *current);

private slots:
	void on_btnClose_clicked();
	void on_wgtDefaultPresentationStyle_sigPresentationStyleChangedByUser();
	void on_cbEmptySlideBeforeSong_clicked(bool checked);
	void on_cbEmptySlideAfterSong_clicked(bool checked);

private:
	Ui::SettingsDialog *ui;

};

extern SettingsDialog *settingsDialog;

#endif // SETTINGSDIALOG_H
