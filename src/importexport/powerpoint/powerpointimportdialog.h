#pragma once

#include <QDialog>

namespace Ui {
	class PowerPointImportDialog;
}

class PowerPointImportDialog : public QDialog {
	Q_OBJECT

public:
	explicit PowerPointImportDialog(QWidget *parent = nullptr);
	~PowerPointImportDialog();

	static PowerPointImportDialog *instance();

public:
	void show();

private slots:
	void updateUi();

private slots:
	void on_btnImport_clicked();
	void on_btnClose_clicked();

	void on_btnSelectFile_clicked();

private:
	Ui::PowerPointImportDialog *ui;
	QStringList inputFiles_;
};
