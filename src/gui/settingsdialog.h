#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QRect>

#include "rec/presentationstyle.h"

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

	class Settings;

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

public:
	const Settings &settings() const;

private slots:
	void onDisplayChanged(QScreen *current);

private slots:
	void on_btnClose_clicked();

private:
	Ui::SettingsDialog *ui;

private:
	PresentationStyle defaultPresentationStyle_;
	QScopedPointer<Settings> settings_;

};

class SettingsDialog::Settings {

public:
	Settings(SettingsDialog *dlg);

public:
	QRect projectionDisplayGeometry() const;
	const PresentationStyle &defaultPresentationStyle() const;

private:
	SettingsDialog &dlg;

};

extern SettingsDialog *settingsDialog;

#endif // SETTINGSDIALOG_H
