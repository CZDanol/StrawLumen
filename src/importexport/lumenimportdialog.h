#ifndef LUMENIMPORTDIALOG_H
#define LUMENIMPORTDIALOG_H

#include <QDialog>

#include "importexport/exportdb.h"

namespace Ui {
	class LumenImportDialog;
}

class LumenImportDialog : public QDialog {
	Q_OBJECT

public:
	explicit LumenImportDialog(QWidget *parent = nullptr);
	~LumenImportDialog();

public:
	void show();
	void show(const QString &filename);

protected:
	void closeEvent(QCloseEvent *) override;

private:
	void updateUi();
	void loadImportFile();
	void setupDefaultUi();

private slots:
	void on_btnClose_clicked();
	void on_btnImport_clicked();
	void on_btnSelectFile_clicked();

private:
	Ui::LumenImportDialog *ui;
	QString importFilename_;
	QScopedPointer<ExportDatabaseManager> db_;
};

LumenImportDialog *lumenImportDialog();

#endif// LUMENIMPORTDIALOG_H
