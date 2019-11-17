#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QDialog>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <QNetworkReply>

namespace Ui {
	class UpdateManager;
}

class UpdateManager : public QDialog
{
	Q_OBJECT

public:
	explicit UpdateManager(QWidget *parent = nullptr);
	virtual ~UpdateManager() override;

public:
	bool isPortableMode = false;

	void checkForUpdates(int updateChannel = 0);

protected:
	void reject() override;

private slots:
	void on_btnDownload_clicked();

private:
	void show(const QString &newVersion, const QString changeLog, int updateChannel);

private slots:
	void onDownloadFinished();
	void onDownloadProgress(qint64 received, qint64 total);
	void onDownloadReadyRead();

private slots:
	void on_btnCancel_clicked();

private:
	Ui::UpdateManager *ui;
	int updateChannel_;

	QNetworkAccessManager networkAccessManager_;
	QSharedPointer<QTemporaryFile> downloadFile_;
	QSharedPointer<QNetworkReply> networkReply_;

};

UpdateManager *updateManager();

#endif // UPDATEMANAGER_H
