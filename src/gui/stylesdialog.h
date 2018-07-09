#ifndef STYLESDIALOG_H
#define STYLESDIALOG_H

#include <QDialog>
#include <QMenu>
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
	bool showInSelectionMode(PresentationStyle &style);

protected:
	void showEvent(QShowEvent *e) override;
	void reject() override;
	void accept() override;

private:
	void setMgmtMode(bool set);
	void setEditMode(bool set);

	/// Returns false if the user does not wish do close the edit mode
	bool askFinishEditMode();

private slots:
	void fillStyleData();
	void requeryList();
	void updateManipulationButtonsEnabled();

private slots:
	void onStyleChanged();
	void onCurrentStyleChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex);
	void onNameChanged();

private slots:
	void on_btnAdd_clicked();
	void on_btnDiscardChanges_clicked();
	void on_btnSaveChanges_clicked();
	void on_btnEdit_clicked();
	void on_lvList_customContextMenuRequested(const QPoint &pos);
	void on_actionDeleteStyle_triggered();
	void on_lvList_activated(const QModelIndex &index);

private:
	Ui::StylesDialog *ui;
	QMenu *listContextMenu_;
	QSqlQueryModel styleListModel_;
	PresentationStyle presentationStyle_;

private:
	bool isMgmtMode_, isEditMode_ = false;
	qlonglong currentStyleId_ = -1;
	bool currentStyleIsInternal_;

};

extern StylesDialog *stylesDialog;

#endif // STYLESDIALOG_H
