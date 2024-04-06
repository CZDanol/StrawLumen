#ifndef FEEDBACKDIALOG_H
#define FEEDBACKDIALOG_H

#include <QDialog>

namespace Ui {
	class FeedbackDialog;
}

class FeedbackDialog : public QDialog {
	Q_OBJECT

public:
	explicit FeedbackDialog(QWidget *parent = 0);
	~FeedbackDialog();

public:
	void show();

private slots:
	void on_btnCancel_clicked();
	void on_btnSend_clicked();

	void on_btnFacebook_clicked();

private:
	Ui::FeedbackDialog *ui;
};

FeedbackDialog *feedbackDialog();

#endif// FEEDBACKDIALOG_H
