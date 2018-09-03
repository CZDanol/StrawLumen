#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QFileInfo>

#include "gui/settingsdialog.h"
#include "presentation/native/nativeprojectorwindow.h"
#include "gui/backgrounddialog.h"
#include "gui/stylesdialog.h"
#include "gui/mainwindow_songsmode.h"
#include "gui/aboutdialog.h"
#include "gui/bulkeditsongsdialog.h"
#include "importexport/documentgenerationdialog.h"
#include "importexport/lumenimportdialog.h"
#include "importexport/opensongimportdialog.h"
#include "presentation/powerpoint/presentation_powerpoint.h"
#include "presentation/native/presentation_images.h"
#include "presentation/video/presentation_video.h"
#include "strawapi/feedbackdialog.h"
#include "job/db.h"
#include "job/settings.h"
#include "rec/playlist.h"
#include "util/guianimations.h"
#include "util/execonmainthread.h"
#include "util/standarddialogs.h"

MainWindow *mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->swMenu->addWidget(ui->wgtPresentationMode->menuWidget());
	ui->swMenu->addWidget(ui->wgtSongsMode->menuWidget());
	ui->menuProgram->insertMenu(ui->menuProgram->actions()[1], ui->wgtSongsMode->importMenu());
	ui->menuProgram->insertMenu(ui->menuProgram->actions()[2], ui->wgtSongsMode->exportMenu());

	setWindowTitle(QString("Straw Lumen %1").arg(PROGRAM_VERSION));
	restoreGeometry(settings->value("window.mainWindow.geometry").toByteArray());

	connect(db, SIGNAL(sigQueryError(QString,QString)), this, SLOT(onDbQueryError(QString,QString)));

	ui->btnPresentationMode->click();
}

MainWindow::~MainWindow()
{
	settings->setValue("window.mainWindow.geometry", saveGeometry());

	delete ui;
}

void MainWindow::editSong(qlonglong songId)
{
	ui->btnSongsMode->click();
	ui->wgtSongsMode->editSong(songId);
}

void MainWindow::showPresentationMode()
{
	ui->btnPresentationMode->click();
}

void MainWindow::blinkPresentationModeButton()
{
	flashButton(ui->btnPresentationMode);
}

MainWindow_PresentationMode *MainWindow::presentationMode()
{
	return ui->wgtPresentationMode;
}

void MainWindow::showSongsMode()
{
	ui->btnSongsMode->click();
}

MainWindow_SongsMode *MainWindow::songsMode()
{
	return ui->wgtSongsMode;
}

void MainWindow::onDbQueryError(const QString &query, const QString &error)
{
	standardErrorDialog(tr("Chyba databáze: %1\n\n%2").arg(error, query));
}

void MainWindow::onDbDatabaseError(const QString &error)
{
	standardErrorDialog(tr("Chyba databáze: %1").arg(error));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	if(!ui->wgtSongsMode->askFinishEditMode()) {
		e->ignore();
		return;
	}

	if(!ui->wgtPresentationMode->askSaveChanges()) {
		e->ignore();
		return;
	}

	QMainWindow::closeEvent(e);
	emit sigClosed();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if(e->mimeData()->hasUrls()) {
		for(QUrl url : e->mimeData()->urls()) {
			if(!url.isLocalFile())
				return;
		}

		e->setDropAction(Qt::LinkAction);
		e->accept();
	}
}

void MainWindow::dropEvent(QDropEvent *e)
{
	auto urls = e->mimeData()->urls();

	// The exec is there so the explorer the file is dragged from is not frozen while loading
	execOnMainThread([=]{
		QStringList openSongFiles, imageFiles, videoFiles;
		QString lumenFile;

		for(QUrl url : urls) {
			QString filePath = url.toLocalFile();
			QFileInfo fileInfo(filePath);

			if(filePath.contains("opensong", Qt::CaseInsensitive)) {
				openSongFiles << filePath;
				continue;
			}

			const QString suffix = fileInfo.suffix();

			if(Presentation_Images::validExtensions().contains(suffix.toLower())) {
				imageFiles += filePath;
				continue;
			}

			if(Presentation_Video::validExtensions().contains(suffix.toLower())) {
				videoFiles += filePath;
				continue;
			}

			if(suffix == "strawLumen") {
				lumenFile = filePath;
				continue;
			}

			if(!Presentation_PowerPoint::isPowerpointFile(fileInfo))
				return standardErrorDialog(tr("Soubor \"%1\" není podporován.").arg(filePath));

			if(!presentationMode()->playlist()->addItem(Presentation_PowerPoint::createFromFilename(filePath)))
				return;

			showPresentationMode();
		}

		if(!imageFiles.isEmpty())
			presentationMode()->playlist()->addItem(Presentation_Images::create(imageFiles));

		for(const QString &filename : videoFiles)
			presentationMode()->playlist()->addItem(Presentation_Video::createFromFilename(filename));

		if(!lumenFile.isEmpty())
			lumenImportDialog()->show(lumenFile);

		if(!openSongFiles.isEmpty())
			openSongImportDialog()->show(openSongFiles);
	});
}

void MainWindow::on_actionSettings_triggered()
{
	settingsDialog->show();
}

void MainWindow::on_btnPresentationMode_clicked()
{
	if(!ui->wgtSongsMode->askFinishEditMode())
		return;

	ui->swModes->setCurrentWidget(ui->wgtPresentationMode);
	ui->swMenu->setCurrentWidget(ui->wgtPresentationMode->menuWidget());
}

void MainWindow::on_btnSongsMode_clicked()
{
	ui->swModes->setCurrentWidget(ui->wgtSongsMode);
	ui->swMenu->setCurrentWidget(ui->wgtSongsMode->menuWidget());
}

void MainWindow::on_actionBackgrounds_triggered()
{
	backgroundDialog->showInMgmtMode();
}

void MainWindow::on_actionStyles_triggered()
{
	stylesDialog->showInMgmtMode();
}

void MainWindow::on_actionGenerateDocuments_triggered()
{
	documentGenerationDialog()->show();
}

void MainWindow::on_actionAbout_triggered()
{
	aboutDialog()->show();
}

void MainWindow::on_actionSendFeedback_triggered()
{
	feedbackDialog()->show();
}

void MainWindow::on_actionBulkEditSongs_triggered()
{
	bulkEditSongsDialog()->show();
}
