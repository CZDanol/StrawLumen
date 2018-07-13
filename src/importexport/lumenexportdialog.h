#ifndef LUMENEXPORTDIALOG_H
#define LUMENEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
	class LumenExportDialog;
}

class LumenExportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LumenExportDialog(QWidget *parent = 0);
	~LumenExportDialog();

public:
	void setSelectedSongs(const QVector<qlonglong> &songIds);

private slots:
	void on_btnClose_clicked();
	void on_btnExport_clicked();

private:
	Ui::LumenExportDialog *ui;

};

LumenExportDialog *lumenExportDialog();

#endif // LUMENEXPORTDIALOG_H
