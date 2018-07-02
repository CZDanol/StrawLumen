#include "mainwindow_presentationmode.h"
#include "ui_mainwindow_presentationmode.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QStringList>

#include "presentation/playlist.h"
#include "presentation/powerpointpresentation.h"
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

	ui->tvPlaylist->setModel(&playlistItemModel_);
	ui->tvSlides->setModel(&slidesItemModel_);
	ui->tvSlides->setItemDelegate(&slidesItemDelegate_);
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

			if(!PowerpointPresentation::allowedExtensions.contains(extension))
				return standardErrorDialog(tr("Soubor '%1' s příponou '%2' není podporován.").arg(filename, extension));

			if(!playlist_->addItem(PowerpointPresentation::create(filename)))
				return;
		}
	});
}
