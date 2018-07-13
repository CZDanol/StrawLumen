#ifndef OPENSONGEXPORTDIALOG_H
#define OPENSONGEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
	class OpenSongExportDialog;
}

class OpenSongExportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OpenSongExportDialog(QWidget *parent = 0);
	~OpenSongExportDialog();

public:
	void setSelectedSongs(const QVector<qlonglong> &songIds);

private slots:
	void on_btnClose_clicked();
	void on_btnExport_clicked();

private:
	Ui::OpenSongExportDialog *ui;
};

OpenSongExportDialog *openSongExportDialog();

#endif // OPENSONGEXPORTDIALOG_H
