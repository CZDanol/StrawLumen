#include "mainwindow_songsmode.h"
#include "ui_mainwindow_songsmode.h"

#include <QStyle>
#include <QUuid>
#include <QShortcut>
#include <QMenu>

#include "util/standarddialogs.h"
#include "job/db.h"

// F(ui->control)
#define SONG_FIELDS_FACTORY(F) \
	F(lnName) F(lnAuthor) F(lnSlideOrder) F(teContent)

MainWindow_SongsMode::MainWindow_SongsMode(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow_SongsMode)
{
	ui->setupUi(this);
	ui->twSong->setCornerWidget(ui->twSongCorner);
	ui->twSongs->setCornerWidget(ui->twSongsCorner);

	connect(ui->wgtSongList, SIGNAL(sigSelectionChanged(qlonglong,int)), this, SLOT(onCurrentSongChanged(qlonglong, int)));
	connect(ui->wgtSongList, SIGNAL(sigItemActivated()), ui->btnEdit, SLOT(click()));
	connect(ui->wgtSongList, SIGNAL(sigCustomContextMenuRequested(QPoint)), this, SLOT(onSongListContextMenuRequested(QPoint)));

	new QShortcut(Qt::CTRL | Qt::Key_Return, ui->btnSaveChanges, SLOT(click()));
	new QShortcut(Qt::CTRL | Qt::Key_S, ui->btnSaveChanges, SLOT(click()));
	new QShortcut(Qt::CTRL | Qt::Key_N, ui->btnNew, SLOT(click()));
	new QShortcut(Qt::Key_Escape, ui->btnDiscardChanges, SLOT(click()));

	//connect(new QShortcut(Qt::Key_Delete, this), SIGNAL(activated()), ui->actionDeleteSong, SLOT(trigger()));

	// To force update
	isSongEditMode_ = true;
	setSongEditMode(false);
}

MainWindow_SongsMode::~MainWindow_SongsMode()
{
	delete ui;
}

QWidget *MainWindow_SongsMode::menuWidget()
{
	return ui->wgtMenu;
}

void MainWindow_SongsMode::setSongEditMode(bool set)
{
	if(isSongEditMode_ == set) {
		updateSongManipulationButtonsEnabled();
		return;
	}

	isSongEditMode_ = set;

	ui->btnEdit->setVisible(!set);
	ui->btnNew->setEnabled(!set);
	ui->btnSaveChanges->setVisible(set);
	ui->btnDiscardChanges->setVisible(set);

#define F(control) \
	ui->control->setReadOnly(!set);\
	ui->control->style()->unpolish(ui->control); \
	ui->control->style()->polish(ui->control);

	SONG_FIELDS_FACTORY(F)
#undef F

	updateSongManipulationButtonsEnabled();
}

void MainWindow_SongsMode::updateSongManipulationButtonsEnabled()
{
	const bool enabled = !isSongEditMode_ && currentSongId_ != -1;

	ui->btnEdit->setEnabled(enabled);
	ui->actionDeleteSong->setEnabled(enabled);
}

void MainWindow_SongsMode::onCurrentSongChanged(qlonglong songId, int prevRowId)
{
	if(songId == currentSongId_)
		return;

	if(isSongEditMode_ && !standardConfirmDialog(tr("Aktuální píseň je otevřená pro editaci. Chcete pokračovat a zahodit změny?"))) {
		ui->wgtSongList->selectRow(prevRowId);
		return;
	}

	if(songId == -1)
		return;

	currentSongId_ = songId;

	setSongEditMode(false);

	QSqlRecord r = db->selectRow("SELECT * FROM songs WHERE id = ?", {songId});
	ui->lnName->setText(r.value("name").toString());
	ui->lnAuthor->setText(r.value("author").toString());
	ui->lnSlideOrder->setText(r.value("slideOrder").toString());
	ui->teContent->setText(r.value("content").toString());
}

void MainWindow_SongsMode::onSongListContextMenuRequested(const QPoint &globalPos)
{
	QMenu menu;
	menu.addAction(ui->actionDeleteSong);
	menu.popup(globalPos);
}

void MainWindow_SongsMode::on_btnNew_clicked()
{
	ui->wgtSongList->unselect();

	ui->lnName->setText("Nová písnička");
	ui->lnAuthor->clear();
	ui->lnSlideOrder->clear();
	ui->teContent->clear();

	currentSongId_ = -1;

	setSongEditMode(true);
}

void MainWindow_SongsMode::on_btnDiscardChanges_clicked()
{
	setSongEditMode(false);
	onCurrentSongChanged(currentSongId_, ui->wgtSongList->currentRowId());
}

void MainWindow_SongsMode::on_btnSaveChanges_clicked()
{
	setSongEditMode(false);

	db->beginTransaction();

	if(currentSongId_ == -1)
		currentSongId_ = db->insert("INSERT INTO songs(uid) VALUES(?)", {QUuid::createUuid().toString()}).toLongLong();
	else
		db->exec("DELETE FROM songs_fulltext WHERE docid = ?", {currentSongId_});

	const QString name = ui->lnName->text();
	const QString author = ui->lnAuthor->text();
	const QString content = ui->teContent->toPlainText();

	db->exec("UPDATE songs SET name = ?, author = ?, content = ?, slideOrder = ? WHERE id = ?", {name, author, content, ui->lnSlideOrder->text(), currentSongId_});
	db->exec("INSERT INTO songs_fulltext(docid, name, author, content) VALUES(?, ?, ?, ?)", {currentSongId_, collate(name), collate(author), collate(content)});
	db->commitTransaction();

	ui->wgtSongList->requery();
	updateSongManipulationButtonsEnabled();
}

void MainWindow_SongsMode::on_btnEdit_clicked()
{
	setSongEditMode(true);
	ui->lnName->setFocus();
}

void MainWindow_SongsMode::on_actionDeleteSong_triggered()
{
	if(!deleteConfirmDialog(tr("Opravdu smazat píseň \"%1\"?").arg(ui->lnName->text())))
		return;

	db->beginTransaction();
	db->exec("DELETE FROM songs_fulltext WHERE docid = ?", {currentSongId_});
	db->exec("DELETE FROM songs WHERE id = ?", {currentSongId_});
	db->commitTransaction();

	ui->wgtSongList->requery();
}
