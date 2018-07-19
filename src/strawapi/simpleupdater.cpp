#include "simpleupdater.h"
#include "ui_simpleupdater.h"

#include <QtConcurrent/QtConcurrent>
#include <QJsonDocument>

#include "gui/mainwindow.h"
#include "util/execonmainthread.h"
#include "util/standarddialogs.h"
#include "strawapi/strawapi.h"

SimpleUpdater::SimpleUpdater(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SimpleUpdater)
{
	ui->setupUi(this);
}

SimpleUpdater::~SimpleUpdater()
{
	delete ui;
}

void SimpleUpdater::checkForUpdates()
{
	QPointer<SimpleUpdater> ptr(this);
	QtConcurrent::run([this, ptr]{
		const QJsonObject request {
			{"action", "checkForSoftwareUpdates"},
			{"productKey", PRODUCT_IDSTR},
			{"currentVersion", PROGRAM_VERSION},
			{"platform", PLATFORM_ID}
		};

		QJsonObject response;
		auto result = StrawApi::requestJson(request, response);

		if(result != StrawApi::RequestResult::ok)
			return;

		if(response["status"] != "ok") {
			qWarning() << "(update check) Update check problem: " << response["error"].toString();
			return;
		}

		if(response["result"] == "updateAvailable" && response["newVersion"] != UPSTREAM_VERSION) {
			const QString newVersion = response["newVersion"].toString();
			const QString changeLog = response["changeLog"].toString();

			execOnMainThread([this, ptr, newVersion, changeLog]{
				if(ptr.isNull())
					return;

				show(newVersion, changeLog);
			});
		}

		else
			qDebug() << "(update check) No updates available";
	});
}

void SimpleUpdater::reject()
{
	networkReply_.reset();
	QDialog::reject();
}

void SimpleUpdater::show(const QString &newVersion, const QString changeLog)
{
	Q_UNUSED(newVersion);

	QTextDocument *doc = ui->tbChangelog->document();
	QFile styleSheet(":/changelogStylesheet.css");
	styleSheet.read(QIODevice::ReadOnly);

	doc->setDefaultStyleSheet(QString::fromUtf8(styleSheet.readAll()));
	doc->setHtml(changeLog);

	ui->tbChangelog->setDocument(doc);
	ui->pbProgress->setValue(0);
	ui->pbProgress->setEnabled(false);
	ui->btnDownload->setEnabled(true);

	QDialog::show();
}

void SimpleUpdater::onDownloadFinished()
{
	accept();

	downloadFile_->reset();
	qDebug() << downloadFile_->readAll();

	if(networkReply_->error() != QNetworkReply::NoError)
		return standardErrorDialog(tr("Při stahování aktualizace nastala chyba: %1").arg(networkReply_->errorString()));

	const QString newFileName = QDir::temp().absoluteFilePath(QString("%1.update.exe").arg(downloadFile_->fileName()));
	if(!downloadFile_->rename(newFileName))
		return standardErrorDialog(tr("Nepodařilo se připravit soubor aktualizace."));

	if(!QProcess::startDetached(newFileName, {}))
		 return standardErrorDialog(tr("Nepodařilo se spustit aktualizaci."));
}

void SimpleUpdater::onDownloadProgress(qint64 received, qint64 total)
{
	double progress = double( received ) / total;
	ui->pbProgress->setValue( progress * 1000.0 );
}

void SimpleUpdater::onDownloadReadyRead()
{
	downloadFile_->write(networkReply_->readAll());
}

SimpleUpdater *simpleUpdater()
{
	static SimpleUpdater *dlg = nullptr;
	if(!dlg)
		dlg = new SimpleUpdater(mainWindow);

	return dlg;
}

void SimpleUpdater::on_btnDownload_clicked()
{
	downloadFile_.reset(new QTemporaryFile());
	if(!downloadFile_->open())
		return standardErrorDialog(tr("Nepodařilo se připravit místo pro stahování."));

	const QJsonObject request {
		{"action", "downloadSoftware"},
		{"productKey", PRODUCT_IDSTR},
		{"platform", PLATFORM_ID}
	};

	QNetworkRequest req(StrawApi::apiUrl);
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	req.setHeader(QNetworkRequest::UserAgentHeader, "StrawSolutionsAPI/1.0");

	networkReply_.reset(networkAccessManager_.post(req, QJsonDocument(request).toJson(QJsonDocument::Compact)));
	connect(networkReply_.data(),SIGNAL(finished()),this,SLOT(onDownloadFinished()));
	connect(networkReply_.data(),SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(onDownloadProgress(qint64,qint64)));
	connect(networkReply_.data(),SIGNAL(readyRead()),this,SLOT(onDownloadReadyRead()));

	ui->btnDownload->setEnabled(false);
	ui->pbProgress->setEnabled(true);
}

void SimpleUpdater::on_btnCancel_clicked()
{
	reject();
}
