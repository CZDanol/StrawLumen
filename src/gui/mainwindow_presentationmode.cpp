#include "mainwindow_presentationmode.h"
#include "gui/mainwindow_songsmode.h"
#include "ui_mainwindow_presentationmode.h"

#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QShortcut>
#include <QStandardPaths>
#include <QStringList>

#include "gui/mainwindow.h"
#include "gui/playlistsdialog.h"
#include "gui/quickbibleversewindow.h"
#include "job/db.h"
#include "job/settings.h"
#include "presentation/native/presentation_bibleverse.h"
#include "presentation/native/presentation_blackscreen.h"
#include "presentation/native/presentation_customslide.h"
#include "presentation/native/presentation_images.h"
#include "presentation/native/presentation_song.h"
#include "presentation/powerpoint/presentation_powerpoint.h"
#include "presentation/presentationmanager.h"
#include "presentation/presentationpropertieswidget.h"
#include "presentation/video/presentation_video.h"
#include "presentation/web/presentation_web.h"
#include "rec/playlist.h"
#include "util/guianimations.h"
#include "util/standarddialogs.h"

MainWindow_PresentationMode::MainWindow_PresentationMode(QWidget *parent) : QWidget(parent),
                                                                            ui(new Ui::MainWindow_PresentationMode) {
	ui->setupUi(this);
	setAcceptDrops(true);

	playlist_.reset(new Playlist());
	connect(playlist_.data(), SIGNAL(sigSlidesChanged()), this, SLOT(updateControlsUIEnabled()));

	ui->wgtVideoControl->hide();

	// Playlist tab
	{
		ui->twPlaylist->setCornerWidget(ui->twPlaylistCorner);

		// Model & selection
		{
			playlistItemModel_.setPlaylist(playlist_);
			ui->tvPlaylist->setModel(&playlistItemModel_);

			auto header = ui->tvPlaylist->header();
			header->setSectionResizeMode(0, QHeaderView::Fixed);
			header->resizeSection(0, 24);
			header->setSectionResizeMode(1, QHeaderView::Stretch);

			connect(&playlistItemModel_, SIGNAL(sigForceSelection(int, int)), this, SLOT(onPlaylistForceSelection(int, int)));
			connect(&playlistItemModel_, SIGNAL(modelReset()), this, SLOT(onPlaylistModelReset()));
			connect(ui->tvPlaylist->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onPresentationSelected(QModelIndex)));
			connect(playlist_.data(), &Playlist::sigNameChanged, this, &MainWindow_PresentationMode::onPlaylistManipulatedAnyhow);
			connect(playlist_.data(), &Playlist::sigChanged, this, &MainWindow_PresentationMode::onPlaylistManipulatedAnyhow);
			connect(playlist_.data(), &Playlist::sigSaved, this, &MainWindow_PresentationMode::onPlaylistManipulatedAnyhow);

			onPlaylistManipulatedAnyhow();
		}

		// Menu & controls
		{
			addPresentationMenu_.addAction(ui->actionAddSong);
			addPresentationMenu_.addAction(ui->actionAddPowerpointPresentation);
			addPresentationMenu_.addAction(ui->actionAddBibleVerse);
			addPresentationMenu_.addSeparator();
			addPresentationMenu_.addAction(ui->actionAddImagesPresentation);
			addPresentationMenu_.addAction(ui->actionAddVideoPresentation);
			addPresentationMenu_.addAction(ui->actionAddWebPresentation);
			addPresentationMenu_.addSeparator();
			addPresentationMenu_.addAction(ui->actionAddCustomSlidePresentation);
			addPresentationMenu_.addAction(ui->actionAddBlackScreen);

			ui->btnAddPresentation->setMenu(&addPresentationMenu_);
			connect(&addPresentationMenu_, SIGNAL(aboutToShow()), this, SLOT(onAddPresentationMenuAboutToShow()));

			insertPresentationBeforeMenu_.setTitle(tr("Přidat před:"));
			insertPresentationBeforeMenu_.setIcon(QIcon(":/icons/16/Upload_16px.png"));
			insertPresentationBeforeMenu_.addActions(addPresentationMenu_.actions());
			connect(&insertPresentationBeforeMenu_, SIGNAL(aboutToShow()), this, SLOT(onAddPresentationBeforeMenuAboutToShow()));

			insertPresentationAfterMenu_.setTitle(tr("Přidat za:"));
			insertPresentationAfterMenu_.setIcon(QIcon(":/icons/16/Download_16px.png"));
			insertPresentationAfterMenu_.addActions(addPresentationMenu_.actions());
			connect(&insertPresentationAfterMenu_, SIGNAL(aboutToShow()), this, SLOT(onAddPresentationAfterMenuAboutToShow()));

			playlistContextMenu_.addAction(ui->actionEditPresentation);
			playlistContextMenu_.addAction(ui->actionDeletePresentation);
			playlistContextMenu_.addMenu(&insertPresentationBeforeMenu_);
			playlistContextMenu_.addMenu(&insertPresentationAfterMenu_);
			connect(ui->tvPlaylist, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onPlaylistContextMenuRequested(QPoint)));

			ui->btnPlaylists->setMenu(&playlistsMenu_);

			playlistsMenu_.setTitle(tr("Program"));
			playlistsMenu_.addAction(ui->actionClearPlaylist);
			playlistsMenu_.addSeparator();
			playlistsMenu_.addAction(ui->actionSavePlaylist);
			playlistsMenu_.addAction(ui->actionSavePlaylistAs);
			playlistsMenu_.addSeparator();
			playlistsMenu_.addAction(ui->actionLoadPlaylist);
			playlistsMenu_.addMenu(&playlistsRecentMenu_);

			playlistsRecentMenu_.setTitle(tr("Načíst nedávné"));
			playlistsRecentMenu_.setIcon(QIcon(":/icons/16/Open_16px.png"));
		}
	}

	// Slides tab
	{
		slidesItemModel_.setPlaylist(playlist_);

		ui->tvSlides->setModel(&slidesItemModel_);
		ui->tvSlides->setItemDelegate(&slidesItemDelegate_);

		auto header = ui->tvSlides->header();
		header->setSectionResizeMode(0, QHeaderView::Fixed);
		header->resizeSection(0, 164);
		header->setSectionResizeMode(1, QHeaderView::Fixed);
		header->resizeSection(1, 64);
		header->setSectionResizeMode(2, QHeaderView::Stretch);

		connect(ui->tvSlides->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(onSlideSelected(QModelIndex)));
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

		connect(ui->btnPreviousPresentation, SIGNAL(clicked()), this, SLOT(scrollToCurrentSlideInSlidesView()));
		connect(ui->btnPreviousSlide, SIGNAL(clicked()), this, SLOT(scrollToCurrentSlideInSlidesView()));
		connect(ui->btnNextSlide, SIGNAL(clicked()), this, SLOT(scrollToCurrentSlideInSlidesView()));
		connect(ui->btnNextPresentation, SIGNAL(clicked()), this, SLOT(scrollToCurrentSlideInSlidesView()));
	}

	// Song list
	{
		ui->wgtSongList->setShowTags(true);

		connect(ui->wgtSongList, SIGNAL(sigItemActivated(qlonglong)), this, SLOT(onSongListItemActivated(qlonglong)));
		connect(ui->wgtSongList, SIGNAL(sigCustomContextMenuRequested(QPoint)), this, SLOT(onSongListContextMenuRequested(QPoint)));

		songListContextMenu_.addAction(ui->actionAddSongsToPlaylist);
		songListContextMenu_.addAction(ui->actionEditSong);
		songListContextMenu_.addAction(ui->actionDeleteSongs);
	}

	// Shortcuts
	{
		// Pageup/down cannot be connected directly using qshortcut native thingies because when the buttons are not enabled, the shortcuts don't prop
		// So for example in slide list, pageUp/down then have their default behavior, which is bad
		connect(new QShortcut(Qt::Key_PageDown, this), SIGNAL(activated()), ui->btnNextSlide, SLOT(click()));
		connect(new QShortcut(Qt::Key_PageUp, this), SIGNAL(activated()), ui->btnPreviousSlide, SLOT(click()));
		new QShortcut(Qt::Key_F5, ui->btnEnableProjection, SLOT(click()));
		new QShortcut(Qt::Key_B, ui->btnBlackScreen, SLOT(click()));
		new QShortcut(Qt::Key_Escape, this, SLOT(disablePresentation()));

		connect(new QShortcut(Qt::Key_Delete, ui->tvPlaylist, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), ui->actionDeletePresentation, SLOT(trigger()));
	}

	ui->twLeftBottom->setCornerWidget(ui->twLeftBottomCorner);
	ui->twLeftBottom->setTabEnabled(ui->twLeftBottom->indexOf(ui->tabPresentationProperties), false);

	isTwLeftBottomHidden_ = true;
	ui->btnShowHideTwLeftBottom->click();

	updateControlsUIEnabled();
}

MainWindow_PresentationMode::~MainWindow_PresentationMode() {
	delete ui;
}

QWidget *MainWindow_PresentationMode::menuWidget() {
	return ui->wgtMenu;
}

VideoControlTabWidget *MainWindow_PresentationMode::videoControlWidget() {
	return ui->wgtVideoControl;
}

bool MainWindow_PresentationMode::askSaveChanges() {
	if(!playlist_->items().count() || playlist_->areChangesSaved())
		return true;

	if(!standardConfirmDialog(tr("Program promítání obsahuje neuložené změny. Opravdu chcete pokračovat?")))
		return false;

	return true;
}

QSharedPointer<Playlist> MainWindow_PresentationMode::playlist() {
	return playlist_;
}

QVector<qlonglong> MainWindow_PresentationMode::selectedSongIds() {
	return ui->wgtSongList->selectedRowIds();
}

void MainWindow_PresentationMode::updatePlaylistsMenu() {
	playlistsRecentMenu_.clear();

	QSqlQuery q = db->selectQuery("SELECT id, name, lastTouch FROM playlists ORDER BY lastTouch DESC LIMIT 5");
	while(q.next()) {
		const qlonglong id = q.value("id").toLongLong();
		playlistsRecentMenu_.addAction(q.value("name").toString(), [id] {
			playlistsDialog()->loadPlaylist(id);
		});
	}

	playlistsRecentMenu_.setEnabled(!playlistsRecentMenu_.actions().isEmpty());
}

void MainWindow_PresentationMode::updateControlsUIEnabled() {
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

void MainWindow_PresentationMode::disablePresentation() {
	presentationManager->setActive(false);
}

void MainWindow_PresentationMode::scrollToCurrentSlideInSlidesView() {
	ui->tvSlides->scrollTo(ui->tvSlides->currentIndex(), QAbstractItemView::PositionAtCenter);
}

void MainWindow_PresentationMode::onCurrentTimeTimer() {
	QDateTime currentTime = QDateTime::currentDateTime();
	ui->lblCurrentTime->setText(currentTime.toString(tr("HH:mm:ss")));
	currentTimeTimer_.setInterval(1000 - currentTime.time().msec());
}

void MainWindow_PresentationMode::onPlaylistForceSelection(int first, int last) {
	auto model = ui->tvPlaylist->selectionModel();
	model->clear();

	for(int i = first; i <= last; i++)
		model->select(playlistItemModel_.index(i, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MainWindow_PresentationMode::onMgrCurrentSlideChanged(int globalSlideId) {
	if(globalSlideId >= 0)
		ui->tvSlides->setCurrentIndex(slidesItemModel_.index(globalSlideId, 0));
}

void MainWindow_PresentationMode::onSlideSelected(const QModelIndex &current) {
	presentationManager->setSlide(playlist_.data(), current.row());

	auto presentation = presentationManager->currentPresentation();
	if(presentation && !disablePlaylistSelectionChange_)
		ui->tvPlaylist->setCurrentIndex(playlistItemModel_.index(presentation->positionInPlaylist(), 0));
}

void MainWindow_PresentationMode::onPresentationSelected(const QModelIndex &current) {
	ui->twLeftBottom->setTabEnabled(ui->twLeftBottom->indexOf(ui->tabPresentationProperties), current.isValid());

	if(!current.isValid())
		return;

	auto presentation = playlist_->items()[current.row()];

	if(currentPresentation_ == presentation)
		return;

	currentPresentation_ = presentation;

	if(!isTwLeftBottomHidden_)
		ui->twLeftBottom->setCurrentWidget(ui->tabPresentationProperties);

	if(presentationPropertiesWidget_) {
		if(presentationPropertiesWidget_->presentation() == presentation)
			return;

		delete presentationPropertiesWidget_;
	}

	presentationPropertiesWidget_ = new PresentationPropertiesWidget(presentation, this);
	ui->saPresentationProperties->setWidget(presentationPropertiesWidget_);
}

void MainWindow_PresentationMode::onAfterSlidesViewSlidesChanged() {
	auto presentation = presentationManager->currentPresentation();

	// If the current item was removed
	if(presentation.isNull() || !presentation->playlist())
		return presentationManager->setActive(false);

	// In case presentation slide count changed to ensure to stay in the current presentation
	disablePlaylistSelectionChange_++;
	presentationManager->setSlide(playlist_.data(), presentation->globalSlideIdOffset() + qMin(presentationManager->currentLocalSlideId(), presentation->slideCount() - 1), true);
	disablePlaylistSelectionChange_--;
}

void MainWindow_PresentationMode::onPlaylistModelReset() {
	onPresentationSelected(ui->tvPlaylist->selectionModel()->currentIndex());
}

void MainWindow_PresentationMode::onPlaylistContextMenuRequested(const QPoint &point) {
	if(ui->tvPlaylist->selectionModel()->selectedRows().count())
		playlistContextMenu_.popup(ui->tvPlaylist->viewport()->mapToGlobal(point));
	else
		addPresentationMenu_.popup(ui->tvPlaylist->viewport()->mapToGlobal(point));
}

void MainWindow_PresentationMode::onSongListContextMenuRequested(const QPoint &point) {
	if(!ui->wgtSongList->selectedRowCount())
		return;

	songListContextMenu_.popup(point);
}

void MainWindow_PresentationMode::onSongListItemActivated(qlonglong songId) {
	playlist_->addItem(Presentation_Song::createFromDb(songId));
}

void MainWindow_PresentationMode::onAddPresentationMenuAboutToShow() {
	addPresentationsAction_ = [this](const QVector<QSharedPointer<Presentation>> &presentations) {
		playlist_->addItems(presentations);
		ui->tvPlaylist->setCurrentIndex(playlistItemModel_.index(playlistItemModel_.rowCount(QModelIndex()) - 1, 0));
	};
}

void MainWindow_PresentationMode::onAddPresentationBeforeMenuAboutToShow() {
	const int insertPos = ui->tvPlaylist->currentIndex().row();

	addPresentationsAction_ = [this, insertPos](const QVector<QSharedPointer<Presentation>> &presentations) {
		playlist_->insertItems(insertPos, presentations);
	};
}

void MainWindow_PresentationMode::onAddPresentationAfterMenuAboutToShow() {
	const int insertPos = ui->tvPlaylist->currentIndex().row() + 1;

	addPresentationsAction_ = [this, insertPos](const QVector<QSharedPointer<Presentation>> &presentations) {
		playlist_->insertItems(insertPos, presentations);
	};
}

void MainWindow_PresentationMode::onPlaylistManipulatedAnyhow() {
	ui->twPlaylist->setTabText(0, playlist_->areChangesSaved() ? playlist_->dbName : tr("%1*").arg(playlist_->dbName));

	ui->actionSavePlaylist->setEnabled(!playlist_->areChangesSaved());
}

void MainWindow_PresentationMode::on_btnEnableProjection_clicked(bool checked) {
	if(!checked) {
		presentationManager->setActive(false);
		return;
	}

	int globalSlideId = ui->tvSlides->currentIndex().row();
	if(globalSlideId == -1)
		globalSlideId = 0;

	presentationManager->setSlide(playlist_.data(), globalSlideId);
}

void MainWindow_PresentationMode::on_tvPlaylist_activated(const QModelIndex &index) {
	if(!index.isValid())
		return;

	presentationManager->setSlide(playlist_.data(), playlist_->items()[index.row()]->globalSlideIdOffset());
}

void MainWindow_PresentationMode::on_actionDeletePresentation_triggered() {
	QModelIndexList selection = ui->tvPlaylist->selectionModel()->selectedRows();
	if(selection.isEmpty())
		return;

	if(!standardDeleteConfirmDialog(tr("Opravdu smazat vybrané prezentace?")))
		return;

	QVector<QSharedPointer<Presentation>> items;
	for(const QModelIndex &index: selection)
		items.append(playlist_->items()[index.row()]);

	playlist_->deleteItems(items);
}

void MainWindow_PresentationMode::on_actionAddBlackScreen_triggered() {
	addPresentationsAction_({Presentation_BlackScreen::create()});
}

void MainWindow_PresentationMode::on_actionAddPowerpointPresentation_triggered() {
	static const QIcon icon(":/icons/16/Microsoft PowerPoint_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("PowerPoint prezentace (%1)").arg((Presentation_PowerPoint::validExtensions.isEmpty() ? "" : "*.") + Presentation_PowerPoint::validExtensions.join(" *.")));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import prezentace PowerPoint"));
	dlg.setDirectory(settings->value("dialog.addPowerpointPresentation.directory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("dialog.addPowerpointPresentation.directory", dlg.directory().absolutePath());

	QVector<QSharedPointer<Presentation>> presentations;
	for(auto &filename: dlg.selectedFiles()) {
		QSharedPointer<Presentation> presentation = Presentation_PowerPoint::createFromFilename(filename);
		if(!presentation)
			break;

		presentations << presentation;
	}
	addPresentationsAction_(presentations);
}

#include <QGraphicsDropShadowEffect>

void MainWindow_PresentationMode::on_actionAddSong_triggered() {
	//standardInfoDialog(tr("Vyberte píseň v panelu \"Písně\" vlevo dole a přetáhněte ji do panelu \"Program\"."));
	ui->twLeftBottom->setCurrentWidget(ui->tabSongList);

	if(isTwLeftBottomHidden_)
		ui->btnShowHideTwLeftBottom->click();

	flashWidget(ui->twLeftBottom);
}

void MainWindow_PresentationMode::on_actionEditSong_triggered() {
	mainWindow->showSongsMode();
	mainWindow->songsMode()->editSong(ui->wgtSongList->currentRowId());
}

void MainWindow_PresentationMode::on_actionDeleteSongs_triggered() {
	mainWindow->songsMode()->requestDeleteSongs(ui->wgtSongList->selectedRowIds());
}

void MainWindow_PresentationMode::on_actionAddSongsToPlaylist_triggered() {
	QVector<QSharedPointer<Presentation>> items;
	for(const qlonglong songId: ui->wgtSongList->selectedRowIds())
		items << Presentation_Song::createFromDb(songId);

	playlist_->addItems(items);
}

void MainWindow_PresentationMode::on_btnShowHideTwLeftBottom_clicked() {
	if(isTwLeftBottomHidden_) {
		ui->twLeftBottom->currentWidget()->show();
		ui->twLeftBottom->setMaximumHeight(QWIDGETSIZE_MAX);

		static const QPixmap pixmap(":/icons/16/Expand Arrow_16px.png");
		ui->btnShowHideTwLeftBottom->setIcon(pixmap);
		ui->btnShowHideTwLeftBottom->setText(tr("Skrýt"));
		isTwLeftBottomHidden_ = false;
	}
	else {
		ui->twLeftBottom->currentWidget()->hide();
		ui->twLeftBottom->setMaximumHeight(ui->twLeftBottom->tabBar()->height() + 8);

		static const QPixmap pixmap(":/icons/16/Collapse Arrow_16px.png");
		ui->btnShowHideTwLeftBottom->setIcon(pixmap);
		ui->btnShowHideTwLeftBottom->setText(tr("Zobrazit"));
		isTwLeftBottomHidden_ = true;
	}
}

void MainWindow_PresentationMode::on_twLeftBottom_currentChanged(int) {
	if(isTwLeftBottomHidden_)
		ui->btnShowHideTwLeftBottom->click();
}

void MainWindow_PresentationMode::on_actionAddCustomSlidePresentation_triggered() {
	addPresentationsAction_({Presentation_CustomSlide::create()});
}

void MainWindow_PresentationMode::on_actionEditPresentation_triggered() {
	if(!ui->tabPresentationProperties->isEnabled())
		return;

	ui->twLeftBottom->setCurrentWidget(ui->tabPresentationProperties);
	flashWidget(ui->twLeftBottom);
}

void MainWindow_PresentationMode::on_actionClearPlaylist_triggered() {
	if(!askSaveChanges())
		return;

	playlist_->clear();
	playlist_->assumeChangesSaved();
}

void MainWindow_PresentationMode::on_btnPlaylists_pressed() {
	updatePlaylistsMenu();
}

void MainWindow_PresentationMode::on_actionSavePlaylist_triggered() {
	if(playlist_->dbId != -1)
		playlistsDialog()->saveWorkingPlaylist(playlist_->dbId);
	else
		playlistsDialog()->show(true);
}

void MainWindow_PresentationMode::on_actionLoadPlaylist_triggered() {
	playlistsDialog()->show(false);
}

void MainWindow_PresentationMode::on_actionSavePlaylistAs_triggered() {
	playlistsDialog()->show(true);
}

void MainWindow_PresentationMode::on_actionAddImagesPresentation_triggered() {
	static const QIcon icon(":/icons/16/Add_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Soubory obrázků (%1)").arg("*." + Presentation_Images::validExtensions().join(" *.")));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Přidání obrázků"));
	dlg.setDirectory(settings->value("imagesDirectory", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("imagesDirectory", dlg.directory().absolutePath());

	addPresentationsAction_({Presentation_Images::create(dlg.selectedFiles())});
}

void MainWindow_PresentationMode::on_actionAddVideoPresentation_triggered() {
	static const QIcon icon(":/icons/16/Play Button_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Soubory videa (%1)").arg("*." + Presentation_Video::validExtensions().join(" *.")));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import videa"));
	dlg.setDirectory(settings->value("dialog.addVideoPresentation.directory", QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("dialog.addVideoPresentation.directory", dlg.directory().absolutePath());

	QVector<QSharedPointer<Presentation>> presentations;
	for(auto &filename: dlg.selectedFiles()) {
		QSharedPointer<Presentation> presentation = Presentation_Video::createFromFilename(filename);
		if(!presentation)
			break;

		presentations << presentation;
	}
	addPresentationsAction_(presentations);
}

void MainWindow_PresentationMode::on_actionAddBibleVerse_triggered() {
	addPresentationsAction_({Presentation_BibleVerse::create()});
}

void MainWindow_PresentationMode::on_actionAddWebPresentation_triggered() {
	addPresentationsAction_({Presentation_Web::create()});
}

void MainWindow_PresentationMode::on_btnQuickVerse_clicked() {
	QuickBibleVerseWindow::instance()->show();
}
