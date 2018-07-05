#include "mainwindow_songsmode.h"
#include "ui_mainwindow_songsmode.h"

#include <QStyle>
#include <QUuid>
#include <QShortcut>
#include <QMenu>
#include <QStringList>

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
	new QShortcut(Qt::Key_F2, ui->btnEdit, SLOT(click()));

	connect(new QShortcut(Qt::Key_Delete, this), SIGNAL(activated()), ui->actionDeleteSong, SLOT(trigger()));

	// Slide order completer
	{
		slideOrderCompleter_.setModel(&slideOrderCompleterModel_);
		slideOrderCompleter_.setCaseSensitivity(Qt::CaseInsensitive);

		ui->lnSlideOrder->setCompleter(&slideOrderCompleter_);
	}

	// Insert song section menu
	{
		const QStringList sectionNames {
			"C", "V1", "V2", "V3", "B", "I", "O"
		};

		insertSectionMenu = new QMenu();
		for(auto &sectionName : sectionNames) {
			SongSection section(sectionName);
			insertSectionMenu->addAction(section.icon(), section.userFriendlyName(), [=]{
				insertSongSection(section);
			});
		}

		SongSection customSection = SongSection::customSection("");
		insertSectionMenu->addAction(customSection.icon(), tr("Vlastní název"), [=]{
			insertSongSection(customSection);
		});

		ui->btnInsertSection->setMenu(insertSectionMenu);
	}

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

void MainWindow_SongsMode::editSong(qlonglong songId)
{
	onCurrentSongChanged(songId, ui->wgtSongList->currentRowId());
	ui->btnEdit->click();
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

	ui->twEdit->setVisible(set);
	ui->twTranspose->setVisible(set);

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

void MainWindow_SongsMode::insertSongSection(const SongSection &section)
{
	if(!isSongEditMode_)
		return;

	if(ui->teContent->textCursor().positionInBlock() != 0)
		ui->teContent->insertPlainText("\n");

	ui->teContent->insertPlainText("\n" + section.annotation() + "\n");
	ui->teContent->setFocus();
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
	if(!standardConfirmDialog(tr("Opravdu zahodit provedené úpravy?")))
		return;

	setSongEditMode(false);

	// Force update
	const qlonglong songId = currentSongId_;
	currentSongId_ = -1;
	onCurrentSongChanged(songId, ui->wgtSongList->currentRowId());
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

void MainWindow_SongsMode::on_lnSlideOrder_sigFocused()
{
	QStringList lst;
	for(SongSection &section : SongSection::songSections(ui->teContent->toPlainText()))
		lst.append(section.standardName());

	slideOrderCompleterModel_.setStringList(lst);
}
