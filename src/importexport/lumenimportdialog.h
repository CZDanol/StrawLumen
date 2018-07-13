#ifndef LUMENIMPORTDIALOG_H
#define LUMENIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
	class LumenImportDialog;
}

class LumenImportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LumenImportDialog(QWidget *parent = 0);
	~LumenImportDialog();

public:
	void show();

private:
	void updateUi();

private slots:
	void on_btnClose_clicked();
	void on_btnImport_clicked();
	void on_btnSelectFile_clicked();

private:
	Ui::LumenImportDialog *ui;
	QString importFilename_;

};

LumenImportDialog *lumenImportDialog();

#endif // LUMENIMPORTDIALOG_H
