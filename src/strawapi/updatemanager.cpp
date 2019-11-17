#include "updatemanager.h"
#include "ui_updatemanager.h"

#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>
#include <QTextDocument>
#include <QScrollBar>

#include "gui/mainwindow.h"
#include "util/standarddialogs.h"
#include "strawapi/strawapi.h"
#include "main.h"

#define MULTILINE(...) #__VA_ARGS__

UpdateManager::UpdateManager(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UpdateManager)
{
	ui->setupUi(this);

	QFile styleSheet(":/css/changelog.css");
	styleSheet.open(QIODevice::ReadOnly);
	ui->tbChangelog->document()->setDefaultStyleSheet(QString::fromUtf8(styleSheet.readAll()));
}

UpdateManager::~UpdateManager()
{
	delete ui;
}

void UpdateManager::checkForUpdates(int updateChannel)
{
	QPointer<UpdateManager> ptr(this);
	QtConcurrent::run([ptr, updateChannel]{
		const QJsonObject request {
			{"action", "checkUpdates"},
			{"product", PRODUCT_IDSTR},
			{"currentVersion", PROGRAM_VERSION},
			{"platform", PLATFORM_ID},
			{"updateChannel", updateChannel}
		};

		QJsonObject response;
		auto result = StrawApi::requestJson(request, response);

		if(result != StrawApi::RequestResult::ok)
			return;

		if(response["status"] != "ok") {
			qWarning() << "(update check) Update check problem: " << response["error"].toString();
			return;
		}

		if(response["result"] == "updateAvailable" && response["newVersion"] != PROGRAM_UPSTREAM_VERSION) {
			const QString newVersion = response["newVersion"].toString();
			const QString changeLog = response["changeLog"].toString();

			QTimer::singleShot(0, qApp, [ptr, newVersion, changeLog, updateChannel]{
				if(ptr.isNull())
					return;

				ptr->show(newVersion, changeLog, updateChannel);
			});
		}

		else
			qDebug() << "(update check) No updates available";
	});
}

void UpdateManager::reject()
{
	networkReply_.reset();
	QDialog::reject();
}

void UpdateManager::show(const QString &newVersion, const QString changeLog, int updateChannel)
{
	Q_UNUSED(newVersion)
	updateChannel_ = updateChannel;

	ui->tbChangelog->document()->setHtml(QStringLiteral("<html><body>%1</body></html>").arg(changeLog));

	ui->pbProgress->setValue(0);
	ui->pbProgress->setEnabled(false);
	ui->btnDownload->setEnabled(true);

	QDialog::show();
	ui->tbChangelog->verticalScrollBar()->setValue(ui->tbChangelog->verticalScrollBar()->minimum());
}

void UpdateManager::onDownloadFinished()
{
	accept();

	downloadFile_->reset();

	if(networkReply_->error() != QNetworkReply::NoError)
		return standardErrorDialog(tr("Při stahování aktualizace nastala chyba: %1").arg(networkReply_->errorString()));

	const QString newFileName = QDir::temp().absoluteFilePath(QStringLiteral("%1.update.exe").arg(downloadFile_->fileName()));
	if(!downloadFile_->rename(newFileName))
		return standardErrorDialog(tr("Nepodařilo se připravit soubor aktualizace."));

	QStringList args;
	if(isPortableMode)
		args += {"/DIR=" + QDir(QDir(qApp->applicationDirPath()).absoluteFilePath("../")).canonicalPath(), "/TYPE=portable", "/COMPONENTS="};

	if(!QProcess::startDetached(newFileName, args))
		 return standardErrorDialog(tr("Nepodařilo se spustit aktualizaci."));

	qApp->quit();
}

void UpdateManager::onDownloadProgress(qint64 received, qint64 total)
{
	double progress = double(received) / total;
	ui->pbProgress->setValue(int(progress * 1000.0));
}

void UpdateManager::onDownloadReadyRead()
{
	downloadFile_->write(networkReply_->readAll());
}

UpdateManager *updateManager()
{
	static UpdateManager *dlg = nullptr;
	if(!dlg)
		dlg = new UpdateManager(mainWindow);

	return dlg;
}

void UpdateManager::on_btnDownload_clicked()
{
	downloadFile_.reset(new QTemporaryFile());
	if(!downloadFile_->open())
		return standardErrorDialog(tr("Nepodařilo se připravit místo pro stahování."));

	QUrl url("https://api2.straw-solutions.cz/");
	url.setQuery(QStringLiteral("action=downloadSoftware&product=%1&platform=%2&updateChannel=%3&isUpdate=1").arg(PRODUCT_IDSTR, PLATFORM_ID, QString::number(updateChannel_)));

	QNetworkRequest req(url);
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	req.setHeader(QNetworkRequest::UserAgentHeader, "StrawSolutionsAPI/1.0");

	networkReply_.reset(networkAccessManager_.get(req));
	connect(networkReply_.data(),SIGNAL(finished()),this,SLOT(onDownloadFinished()));
	connect(networkReply_.data(),SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(onDownloadProgress(qint64,qint64)));
	connect(networkReply_.data(),SIGNAL(readyRead()),this,SLOT(onDownloadReadyRead()));

	ui->btnDownload->setEnabled(false);
	ui->pbProgress->setEnabled(true);
}

void UpdateManager::on_btnCancel_clicked()
{
	reject();
}
