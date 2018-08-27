#ifndef BULKEDITSONGSDIALOG_H
#define BULKEDITSONGSDIALOG_H

#include <QDialog>

namespace Ui {
	class BulkEditSongsDialog;
}

class BulkEditSongsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit BulkEditSongsDialog(QWidget *parent = nullptr);
	~BulkEditSongsDialog();

private slots:
	void on_btnStorno_clicked();

	void on_btnGenerate_clicked();

private:
	Ui::BulkEditSongsDialog *ui;

};

BulkEditSongsDialog *bulkEditSongsDialog();

#endif // BULKEDITSONGSDIALOG_H
