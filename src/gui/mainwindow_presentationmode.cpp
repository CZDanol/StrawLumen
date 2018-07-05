#include "mainwindow_presentationmode.h"
#include "ui_mainwindow_presentationmode.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>
#include <QShortcut>
#include <QFileDialog>
#include <QStandardPaths>

#include "gui/presentationproperties/presentationpropertieswidget.h"
#include "gui/settingsdialog.h"
#include "presentation/playlist.h"
#include "presentation/presentationmanager.h"
#include "presentation/presentation_powerpoint.h"
#include "presentation/presentation_blackscreen.h"
#include "job/settings.h"
#include "util/standarddialogs.h"
#include "util/execonmainthread.h"

MainWindow_PresentationMode::MainWindow_PresentationMode(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow_PresentationMode)
{
	ui->setupUi(this);
	setAcceptDrops(true);

	playlist_.reset(new Playlist());
	connect(playlist_.data(), SIGNAL(sigSlidesChanged()), this, SLOT(updateControlsUIEnabled()));

	// Playlist tab
	{
		ui->twPlaylist->setCornerWidget(ui->twPlaylistCorner);

		// Model & selection
		{
			playlistItemModel_.setPlaylist(playlist_);
			ui->tvPlaylist->setModel(&playlistItemModel_);

			ui->tvPlaylist->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
			ui->tvPlaylist->header()->setSectionResizeMode(1, QHeaderView::Stretch);

			connect(&playlistItemModel_, SIGNAL(sigForceSelection(int,int)), this, SLOT(onPlaylistForceSelection(int,int)));
			connect(&playlistItemModel_, SIGNAL(modelReset()), this, SLOT(onPlaylistModelReset()));
			connect(ui->tvPlaylist->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onPresentationSelected(QModelIndex)));
		}

		// Menu & controls
		{
			playlistContextMenu_ = new QMenu(this);
			playlistContextMenu_->addAction(ui->actionDeletePresentation);

			addPresentationMenu_ = new QMenu(this);
			addPresentationMenu_->addAction(ui->actionAddPowerpointPresentation);
			addPresentationMenu_->addAction(ui->actionAddBlackScreen);

			ui->btnAddPresentation->setMenu(addPresentationMenu_);

			connect(ui->tvPlaylist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onPlaylistContextMenuRequested(QPoint)));
		}
	}

	// Slides tab
	{
		slidesItemModel_.setPlaylist(playlist_);

		ui->tvSlides->setModel(&slidesItemModel_);
		ui->tvSlides->setItemDelegate(&slidesItemDelegate_);
		ui->tvSlides->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		ui->tvSlides->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		ui->tvSlides->header()->setSectionResizeMode(2, QHeaderView::Stretch);

		connect(ui->tvSlides->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onSlideSelected(QModelIndex)));
		connect(presentationManager, SIGNAL(sigCurrentSlideChanged(int)), this, SLOT(onMgrCurrentSlideChanged(int)));
		connect(&slidesItemModel_, SIGNAL(sigAfterSlidesChanged()), this, SLOT(onAfterSlidesViewSlidesChanged()));
	}

	// Current time tab
	{
		connect(&currentTimeTimer_, SIGNAL(timeout()), this, SLOT(onCurrentTimeTimer()));
		currentTimeTimer_.start();
	}

	// Presentation control tab
	{
		ui->twControls->setCornerWidget(ui->twControlsCorner);

		connect(presentationManager, SIGNAL(sigActiveChanged(bool)), this, SLOT(updateControlsUIEnabled()));
		connect(presentationManager, SIGNAL(sigCurrentSlideChanged(int)), this, SLOT(updateControlsUIEnabled()));
		connect(presentationManager, SIGNAL(sigBlackScreenChanged(bool)), ui->btnBlackScreen, SLOT(setChecked(bool)));

		connect(ui->btnPreviousPresentation, SIGNAL(clicked(bool)), presentationManager, SLOT(previousPresentation()));
		connect(ui->btnPreviousSlide, SIGNAL(clicked(bool)), presentationManager, SLOT(previousSlide()));
		connect(ui->btnNextSlide, SIGNAL(clicked(bool)), presentationManager, SLOT(nextSlide()));
		connect(ui->btnNextPresentation, SIGNAL(clicked(bool)), presentationManager, SLOT(nextPresentation()));
		connect(ui->btnBlackScreen, SIGNAL(clicked(bool)), presentationManager, SLOT(setBlackScreen(bool)));
	}

	// Shortcuts
	{
		new QShortcut(Qt::Key_PageDown, ui->btnNextSlide, SLOT(click()));
		new QShortcut(Qt::Key_PageUp, ui->btnPreviousSlide, SLOT(click()));
		new QShortcut(Qt::Key_B, ui->btnBlackScreen, SLOT(click()));
		new QShortcut(Qt::Key_Escape, this, SLOT(disablePresentation()));

		connect(new QShortcut(Qt::Key_Delete, ui->tvPlaylist, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), ui->actionDeletePresentation, SLOT(trigger()));
	}

	ui->twLeftBottom->setTabEnabled(ui->twLeftBottom->indexOf(ui->tabPresentationProperties), false);

	updateControlsUIEnabled();
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

	// The exec is there so the explorer the file is dragged from is not frozen while loading
	execOnMainThread([=]{
		for(QUrl url : urls) {
			QString filename = url.toLocalFile();
			QFileInfo fileInfo(filename);

			if(!Presentation_PowerPoint::isPowerpointFile(fileInfo))
				return standardErrorDialog(tr("Soubor \"%1\" není podporován.").arg(filename));

			if(!playlist_->addItem(Presentation_PowerPoint::create(filename)))
				return;
		}
	});
}

void MainWindow_PresentationMode::updateControlsUIEnabled()
{
	bool isActive = presentationManager->isActive();
	bool isFirstSlide = isActive && presentationManager->currentGlobalSlideId() == 0;
	bool isLastSlide = isActive && presentationManager->currentGlobalSlideId() == playlist_->slideCount() - 1;

	ui->btnEnableProjection->setChecked(isActive);
	ui->btnEnableProjection->setEnabled(playlist_->slideCount() > 0);

	ui->btnPreviousPresentation->setEnabled(isActive && !isFirstSlide);
	ui->btnPreviousSlide->setEnabled(isActive && !isFirstSlide);
	ui->btnNextSlide->setEnabled(isActive && !isLastSlide);
	ui->btnNextPresentation->setEnabled(isActive && !isLastSlide);
	ui->btnBlackScreen->setEnabled(isActive);
}

void MainWindow_PresentationMode::disablePresentation()
{
	presentationManager->setActive(false);
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

void MainWindow_PresentationMode::onMgrCurrentSlideChanged(int globalSlideId)
{
	ui->tvSlides->setCurrentIndex(slidesItemModel_.index(globalSlideId, 0));
}

void MainWindow_PresentationMode::onSlideSelected(const QModelIndex &current)
{
	presentationManager->setSlide(playlist_.data(), current.row());

	auto presentation = presentationManager->currentPresentation();
	if(presentation)
		ui->tvPlaylist->setCurrentIndex(playlistItemModel_.index(presentation->positionInPlaylist(), 0));
}

void MainWindow_PresentationMode::onPresentationSelected(const QModelIndex &current)
{
	ui->twLeftBottom->setTabEnabled(ui->twLeftBottom->indexOf(ui->tabPresentationProperties), current.isValid());

	if(!current.isValid())
		return;

	auto presentation = playlist_->items()[current.row()];

	if(presentationPropertiesWidget_) {
		if(presentationPropertiesWidget_->presentation() == presentation)
			return;

		delete presentationPropertiesWidget_;
	}

	presentationPropertiesWidget_ = new PresentationPropertiesWidget(presentation, this);
	ui->saPresentationProperties->setWidget(presentationPropertiesWidget_);
	ui->twLeftBottom->setCurrentWidget(ui->tabPresentationProperties);
}

void MainWindow_PresentationMode::onAfterSlidesViewSlidesChanged()
{
	auto presentation = presentationManager->currentPresentation();

	// If the current item was removed
	if(presentation.isNull() || !presentation->playlist())
		return presentationManager->setActive(false);

	// In case presentation slide count changed to ensure to stay in the current presentation
	presentationManager->setSlide(playlist_.data(), presentation->globalSlideIdOffset() + qMin(presentationManager->currentLocalSlideId(), presentation->slideCount()-1));
}

void MainWindow_PresentationMode::onPlaylistModelReset()
{
	onPresentationSelected(QModelIndex());
}

void MainWindow_PresentationMode::onPlaylistContextMenuRequested(const QPoint &point)
{
	if(!ui->tvPlaylist->selectionModel()->selectedRows().count())
		return;

	playlistContextMenu_->popup(ui->tvPlaylist->viewport()->mapToGlobal(point));
}

void MainWindow_PresentationMode::on_btnEnableProjection_clicked(bool checked)
{
	if(!checked) {
		presentationManager->setActive(false);
		return;
	}

	int globalSlideId = ui->tvSlides->currentIndex().row();
	if(globalSlideId == -1)
		globalSlideId = 0;

	presentationManager->setSlide(playlist_.data(), globalSlideId);
}

void MainWindow_PresentationMode::on_tvPlaylist_activated(const QModelIndex &index)
{
	if(!index.isValid())
		return;

	presentationManager->setSlide(playlist_.data(), playlist_->items()[index.row()]->globalSlideIdOffset());
}

void MainWindow_PresentationMode::on_actionDeletePresentation_triggered()
{
	const int index = ui->tvPlaylist->currentIndex().row();
	if(index == -1)
		return;

	auto presentation = playlist_->items()[index];
	if(!standardDeleteConfirmDialog(tr("Opravdu smazat prezentaci \"%1\"?").arg(presentation->identification())))
		return;

	playlist_->deleteItem(presentation);
}

void MainWindow_PresentationMode::on_actionAddBlackScreen_triggered()
{
	playlist_->addItem(Presentation_BlackScreen::create());
}

void MainWindow_PresentationMode::on_btnSettings_clicked()
{
	settingsDialog->show();
}

void MainWindow_PresentationMode::on_actionAddPowerpointPresentation_triggered()
{
	static const QIcon icon(":/icons/16/Microsoft PowerPoint_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("PowerPoint prezentace (%1)").arg((Presentation_PowerPoint::validExtensions.isEmpty() ? "" : "*.") + Presentation_PowerPoint::validExtensions.join(" *.")));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import prezentace PowerPoint"));
	dlg.setDirectory(settings->value("dialog.addPowerpointPresentation.directory", QStandardPaths::writableLocation(QStandardPaths::DataLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("dialog.addPowerpointPresentation.directory", dlg.directory().absolutePath());

	for(auto &filename : dlg.selectedFiles()) {
		if(!playlist_->addItem(Presentation_PowerPoint::create(filename)))
			break;
	}
}
