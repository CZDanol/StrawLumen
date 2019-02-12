#ifndef DOCUMENTGENERATIONDIALOG_H
#define DOCUMENTGENERATIONDIALOG_H

#include <QDialog>
#include <QWebEnginePage>
#include <QWebEngineProfile>

namespace Ui {
	class DocumentGenerationDialog;
}

class DocumentGenerationDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DocumentGenerationDialog(QWidget *parent = 0);
	~DocumentGenerationDialog();

public:
	void show();

	void setSelectedSongs(const QVector<qlonglong> &songIds);

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
	QWebEnginePage *webPage_ = nullptr;
	QWebEngineProfile *webProfile_ = nullptr;

private:
	QString jsCode_;
	QString outputFilePath_;

};

DocumentGenerationDialog *documentGenerationDialog();

#endif // DOCUMENTGENERATIONDIALOG_H
