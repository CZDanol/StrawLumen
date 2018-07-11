#ifndef DOCUMENTGENERATIONDIALOG_H
#define DOCUMENTGENERATIONDIALOG_H

#include <QDialog>
#include <QWebEngineView>

namespace Ui {
	class DocumentGenerationDialog;
}

class DocumentGenerationDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DocumentGenerationDialog(QWidget *parent = 0);
	~DocumentGenerationDialog();

private slots:
	void onLoaded();
	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

private:
	Ui::DocumentGenerationDialog *ui;
	/*QWebEngineView view_;
	QString genJs, jqueryJs;*/

};

extern DocumentGenerationDialog *documentGenerationDialog;

#endif // DOCUMENTGENERATIONDIALOG_H
