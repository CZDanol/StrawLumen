#ifndef STYLESDIALOG_H
#define STYLESDIALOG_H

#include <QDialog>

#include <QSqlQueryModel>

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
	void setEditMode(bool set);
	bool tryCloseEditMode();

private slots:
	void onStyleChanged();
	void onCurrentStyleChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex);
	void fillStyleData();

private slots:
	void on_widget_sigTextStyleChangedByUser(const TextStyle &);
	void on_btnAdd_clicked();
	void on_btnDiscardChanges_clicked();

private:
	Ui::StylesDialog *ui;
	QSqlQueryModel styleListModel_;
	PresentationStyle presentationStyle_;

private:
	bool isMgmtMode_, isEditMode_ = false;
	qlonglong currentStyleId_;

};

extern StylesDialog *stylesDialog;

#endif // STYLESDIALOG_H
