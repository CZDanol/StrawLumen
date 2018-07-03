#ifndef ACTIVEXDEBUGDIALOG_H
#define ACTIVEXDEBUGDIALOG_H

#include <QDialog>

namespace Ui {
	class ActiveXDebugDialog;
}

class ActiveXDebugDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ActiveXDebugDialog(QWidget *parent = 0);
	~ActiveXDebugDialog();

public:
	void show(QString html);

private:
	Ui::ActiveXDebugDialog *ui;

};

extern ActiveXDebugDialog *activeXDebugDialog;

#endif // ACTIVEXDEBUGDIALOG_H
