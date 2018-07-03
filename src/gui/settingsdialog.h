#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QRect>

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

public:
	QRect projectionDisplayGeometry();

private slots:
	void onDisplayChanged(QScreen *current);

private slots:
	void on_btnClose_clicked();

private:
	Ui::SettingsDialog *ui;

};

extern SettingsDialog *settingsDialog;

#endif // SETTINGSDIALOG_H
