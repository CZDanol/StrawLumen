#ifndef STYLESDIALOG_H
#define STYLESDIALOG_H

#include <QDialog>

namespace Ui {
	class StylesDialog;
}

class StylesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StylesDialog(QWidget *parent = 0);
	~StylesDialog();

public:
	void showInMgmtMode();

private:
	void setMgmtMode(bool set);

private:
	Ui::StylesDialog *ui;
	bool isMgmtMode_;

};

extern StylesDialog *stylesDialog;

#endif // STYLESDIALOG_H
