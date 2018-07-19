#ifndef SIMPLEUPDATER_H
#define SIMPLEUPDATER_H

#include <QDialog>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <QNetworkReply>

namespace Ui {
	class SimpleUpdater;
}

class SimpleUpdater : public QDialog
{
	Q_OBJECT

public:
	explicit SimpleUpdater(QWidget *parent = 0);
	~SimpleUpdater();

public:
	void checkForUpdates();

protected:
	void reject() override;

private slots:
	void on_btnDownload_clicked();

private:
	void show(const QString &newVersion, const QString changeLog);

private slots:
	void onDownloadFinished();
	void onDownloadProgress(qint64 received, qint64 total);
	void onDownloadReadyRead();

private slots:
	void on_btnCancel_clicked();

private:
	Ui::SimpleUpdater *ui;
	QNetworkAccessManager networkAccessManager_;
	QSharedPointer<QTemporaryFile> downloadFile_;
	QSharedPointer<QNetworkReply> networkReply_;

};

SimpleUpdater *simpleUpdater();

#endif // SIMPLEUPDATER_H
