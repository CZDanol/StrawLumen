#ifndef STYLESDIALOG_H
#define STYLESDIALOG_H

#include <QDialog>

#include "rec/presentationstyle.h"
#include "rec/textstyle.h"

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

private slots:
	void onStyleChanged();

private slots:
	void on_widget_sigTextStyleChangedByUser(const TextStyle &);

private:
	Ui::StylesDialog *ui;
	PresentationStyle presentationStyle_;
	bool isMgmtMode_;

};

extern StylesDialog *stylesDialog;

#endif // STYLESDIALOG_H
