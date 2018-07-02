#include "mainwindow_presentationmode.h"
#include "ui_mainwindow_presentationmode.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>

#include "presentation/playlist.h"
#include "presentation/presentationmanager.h"
#include "presentation/presentation_powerpoint.h"
#include "util/standarddialogs.h"
#include "util/execonmainthread.h"

MainWindow_PresentationMode::MainWindow_PresentationMode(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow_PresentationMode)
{
	ui->setupUi(this);
	setAcceptDrops(true);

	ui->twPlaylist->setCornerWidget(ui->twPlaylistCorner);

	playlist_.reset(new Playlist());
	playlistItemModel_.setPlaylist(playlist_);
	slidesItemModel_.setPlaylist(playlist_);

	connect(&playlistItemModel_, SIGNAL(sigForceSelection(int,int)), this, SLOT(onPlaylistForceSelection(int,int)));

	ui->tvPlaylist->setModel(&playlistItemModel_);
	ui->tvSlides->setModel(&slidesItemModel_);
	ui->tvSlides->setItemDelegate(&slidesItemDelegate_);

	connect(&currentTimeTimer_, SIGNAL(timeout()), this, SLOT(onCurrentTimeTimer()));
	currentTimeTimer_.start();

	on_btnEnableProjection_toggled(false);
}

MainWindow_PresentationMode::~MainWindow_PresentationMode()
{
	delete ui;
}

QWidget *MainWindow_PresentationMode::menuWidget()
{
	return ui->wgtMenu;
}

void MainWindow_PresentationMode::dragEnterEvent(QDragEnterEvent *e)
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

void MainWindow_PresentationMode::dropEvent(QDropEvent *e)
{
	auto urls = e->mimeData()->urls();

	execOnMainThread([=]{
		for(QUrl url : urls) {
			QString filename = url.toLocalFile();
			QString extension = QFileInfo(filename).suffix();

			if(!Presentation_PowerPoint::allowedExtensions.contains(extension))
				return standardErrorDialog(tr("Soubor '%1' s příponou '%2' není podporován.").arg(filename, extension));

			if(!playlist_->addItem(Presentation_PowerPoint::create(filename)))
				return;
		}
	});
}

void MainWindow_PresentationMode::onCurrentTimeTimer()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	ui->lblCurrentTime->setText(currentTime.toString(tr("HH:mm:ss")));
	currentTimeTimer_.setInterval(1000-currentTime.time().msec());
}

void MainWindow_PresentationMode::onPlaylistForceSelection(int first, int last)
{
	auto model = ui->tvPlaylist->selectionModel();
	model->clear();

	for(int i = first; i <= last; i ++)
		model->select(playlistItemModel_.index(i, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow_PresentationMode::on_btnEnableProjection_toggled(bool checked)
{
	ui->btnPreviousPresentation->setEnabled(checked);
	ui->btnPreviousSlide->setEnabled(checked);
	ui->btnNextSlide->setEnabled(checked);
	ui->btnNextPresentation->setEnabled(checked);
	ui->btnBlackScreen->setEnabled(checked);
}

void MainWindow_PresentationMode::on_tvSlides_activated(const QModelIndex &index)
{
	if(!index.isValid()) {
		presentationManager->setPresentation(nullptr);
		return;
	}

	int globalSlideId = index.row();
	auto presentation = playlist_->presentationOfSlide(globalSlideId);
	int localSlideId = globalSlideId - presentation->firstSlideOffsetInPlaylist();

	presentationManager->setPresentation(presentation);
}
