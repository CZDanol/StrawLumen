#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
	class AboutDialog;
}

class AboutDialog : public QDialog {
	Q_OBJECT

public:
	explicit AboutDialog(QWidget *parent = 0);
	~AboutDialog();

private slots:
	void on_btnFacebookPage_clicked();

	void on_btnWeb_clicked();

	void on_btnChangelog_clicked();

private:
	Ui::AboutDialog *ui;
};

AboutDialog *aboutDialog();

#endif// ABOUTDIALOG_H
