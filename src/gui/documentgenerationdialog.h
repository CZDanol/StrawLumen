#ifndef DOCUMENTGENERATIONDIALOG_H
#define DOCUMENTGENERATIONDIALOG_H

#include <QDialog>
#include <QWebEnginePage>
#include <QEventLoop>

namespace Ui {
	class DocumentGenerationDialog;
}

class DocumentGenerationDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DocumentGenerationDialog(QWidget *parent = 0);
	~DocumentGenerationDialog();

protected:
	void showEvent(QShowEvent *e) override;

private:
	void generate(const QVector<qlonglong> &songIds);
	void generateSong(qlonglong songId, QJsonArray &output);

private slots:
	void onPageLoaded(bool result);
	void onPdfGenerated(const QByteArray &data);

private slots:
	void on_btnStorno_clicked();
	void on_btnGenerate_clicked();

private:
	Ui::DocumentGenerationDialog *ui;

private:
	QWebEnginePage *page_ = nullptr;
	QString jsCode_;
	QString outputFilePath_;

};

extern DocumentGenerationDialog *documentGenerationDialog;

#endif // DOCUMENTGENERATIONDIALOG_H
