#ifndef OPENSONGIMPORTDIALOG_H
#define OPENSONGIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
	class OpenSongImportDialog;
}

class OpenSongImportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OpenSongImportDialog(QWidget *parent = 0);
	~OpenSongImportDialog();

public:
	void show();

private:
	void updateUi();

private slots:
	void on_btnClose_clicked();
	void on_btnImport_clicked();
	void on_btnSelectFiles_clicked();

private:
	Ui::OpenSongImportDialog *ui;
	QStringList importFiles_;

};

OpenSongImportDialog *openSongImportDialog();

#endif // OPENSONGIMPORTDIALOG_H
