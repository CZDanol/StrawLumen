#ifndef BIBLETRANSLATIONMGMTDIALOG_H
#define BIBLETRANSLATIONMGMTDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
	class BibleTranslationMgmtDialog;
}

class BibleTranslationMgmtDialog : public QDialog {
	Q_OBJECT

public:
	explicit BibleTranslationMgmtDialog(QWidget *parent = nullptr);
	~BibleTranslationMgmtDialog();

public slots:
	void updateList();
	void updateUiEnabled();

private slots:
	void on_btnDelete_clicked();

	void on_btnImport_clicked();

	void on_btnClose_clicked();

private:
	Ui::BibleTranslationMgmtDialog *ui;
};

BibleTranslationMgmtDialog *bibleTranslationMgmtDialog();

#endif// BIBLETRANSLATIONMGMTDIALOG_H
