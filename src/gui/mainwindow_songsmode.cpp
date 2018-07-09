#include "mainwindow_songsmode.h"
#include "ui_mainwindow_songsmode.h"

#include <QStyle>
#include <QUuid>
#include <QShortcut>
#include <QMenu>
#include <QStringList>
#include <QDateTime>

#include "util/standarddialogs.h"
#include "rec/chord.h"
#include "job/db.h"

// F(uiControl)
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
	connect(ui->wgtSongList, SIGNAL(sigItemActivated(qlonglong)), ui->btnEdit, SLOT(click()));
	connect(ui->wgtSongList, SIGNAL(sigCustomContextMenuRequested(QPoint)), this, SLOT(onSongListContextMenuRequested(QPoint)));

	new QShortcut(Qt::CTRL | Qt::Key_Return, ui->btnSaveChanges, SLOT(click()));
	new QShortcut(Qt::CTRL | Qt::Key_S, ui->btnSaveChanges, SLOT(click()));
	new QShortcut(Qt::CTRL | Qt::Key_N, ui->btnNew, SLOT(click()));
	new QShortcut(Qt::Key_Escape, ui->btnDiscardChanges, SLOT(click()));
	new QShortcut(Qt::Key_F2, ui->btnEdit, SLOT(click()));

	connect(new QShortcut(Qt::Key_Delete, this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), ui->actionDeleteSong, SLOT(trigger()));

	// Slide order
	{
		slideOrderCompleter_.setModel(&slideOrderCompleterModel_);
		slideOrderCompleter_.setCaseSensitivity(Qt::CaseInsensitive);

		slideOrderValidator_.setRegularExpression(songCustomSlideOrderRegex());

		ui->lnSlideOrder->setCompleter(&slideOrderCompleter_);
		ui->lnSlideOrder->setValidator(&slideOrderValidator_);

		addCustomSlideOrderItemMenu_ = new QMenu(this);
		ui->btnAddCustomSlideOrderItem->setMenu(addCustomSlideOrderItemMenu_);
	}

	// Insert song section menu
	{
		const QStringList sectionNames {
			"C", "V1", "V2", "V3", "B", "I", "O"
		};

		insertSectionMenu_ = new QMenu(this);
		for(auto &sectionName : sectionNames) {
			SongSection section(sectionName);
			insertSectionMenu_->addAction(section.icon(), section.userFriendlyName(), [=]{
				insertSongSection(section);
			});
		}

		SongSection customSection = SongSection::customSection("");
		insertSectionMenu_->addAction(customSection.icon(), tr("Vlastní název"), [=]{
			insertSongSection(customSection);
		});

		ui->btnInsertSection->setMenu(insertSectionMenu_);
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

	ui->btnAddCustomSlideOrderItem->setEnabled(set);

#define F(uiControl) \
	ui->uiControl->setReadOnly(!set);\
	ui->uiControl->style()->unpolish(ui->uiControl); \
	ui->uiControl->style()->polish(ui->uiControl);

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

	if(isSongEditMode_ && !standardConfirmDialog(tr("Aktuální píseň je otevřená pro editaci. Chcete pokračovat a zahodit provedené úpravy?"))) {
		ui->wgtSongList->selectRow(prevRowId);
		return;
	}

	if(songId == -1)
		return;

	currentSongId_ = songId;
	setSongEditMode(false);
	fillSongData();
}

void MainWindow_SongsMode::onSongListContextMenuRequested(const QPoint &globalPos)
{
	QMenu menu;
	menu.addAction(ui->actionDeleteSong);
	menu.popup(globalPos);
}

void MainWindow_SongsMode::fillSongData()
{
	if(currentSongId_ == -1)
		return;

	QSqlRecord r = db->selectRow("SELECT * FROM songs WHERE id = ?", {currentSongId_});
	ui->lnName->setText(r.value("name").toString());
	ui->lnAuthor->setText(r.value("author").toString());
	ui->lnSlideOrder->setText(r.value("slideOrder").toString());
	ui->teContent->setText(r.value("content").toString());
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
	fillSongData(); // Load original data
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

	db->exec(
				"UPDATE songs SET name = ?, author = ?, content = ?, slideOrder = ?, lastEdit = ? WHERE id = ?",
				{name, author, content, ui->lnSlideOrder->text(), QDateTime::currentMSecsSinceEpoch(), currentSongId_}
				);
	db->exec("INSERT INTO songs_fulltext(docid, name, author, content) VALUES(?, ?, ?, ?)", {currentSongId_, collate(name), collate(author), collate(content)});
	db->commitTransaction();

	updateSongManipulationButtonsEnabled();
	emit db->sigSongChanged(currentSongId_);
}

void MainWindow_SongsMode::on_btnEdit_clicked()
{
	setSongEditMode(true);
	ui->lnName->setFocus();
}

void MainWindow_SongsMode::on_actionDeleteSong_triggered()
{
	if(currentSongId_ == -1)
		return;

	if(isSongEditMode_)
		return standardErrorDialog(tr("V režimu úprav nelze mazat písně. Ukončete úpravy a zkuste to znovu."));

	if(!standardDeleteConfirmDialog(tr("Opravdu smazat píseň \"%1\"?").arg(ui->lnName->text())))
		return;

	db->beginTransaction();
	db->exec("DELETE FROM songs_fulltext WHERE docid = ?", {currentSongId_});
	db->exec("DELETE FROM songs WHERE id = ?", {currentSongId_});
	db->commitTransaction();

	emit db->sigSongChanged(currentSongId_);
	currentSongId_ = -1;
	updateSongManipulationButtonsEnabled();
}

void MainWindow_SongsMode::on_lnSlideOrder_sigFocused()
{
	QStringList lst;
	for(SongSection &section : songSections(ui->teContent->toPlainText()))
		lst.append(section.standardName());

	slideOrderCompleterModel_.setStringList(lst);
}

void MainWindow_SongsMode::on_btnInsertChord_clicked()
{
	if(!isSongEditMode_)
		return;

	const QString insertStr = "[C]";

	ui->teContent->setFocus();

	QTextCursor tc = ui->teContent->textCursor();
	tc.insertText(insertStr);
	tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, insertStr.length()-1);
	tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, insertStr.length()-2);
	ui->teContent->setTextCursor(tc);
}

void MainWindow_SongsMode::on_btnTransposeUp_clicked()
{
	QString song = ui->teContent->toPlainText();
	transposeSong(song, 1);
	ui->teContent->setPlainText(song);
}

void MainWindow_SongsMode::on_btnTransposeDown_clicked()
{
	QString song = ui->teContent->toPlainText();
	transposeSong(song, -1);
	ui->teContent->setPlainText(song);
}

void MainWindow_SongsMode::on_btnAddCustomSlideOrderItem_pressed()
{
	addCustomSlideOrderItemMenu_->clear();

	for(SongSection section : songSections(ui->teContent->toPlainText()))
		addCustomSlideOrderItemMenu_->addAction(section.icon(), section.userFriendlyName(), [this, section]{
			const QString text = ui->lnSlideOrder->text();
			ui->lnSlideOrder->setText((text.isEmpty() ? "" : text + " ") + section.standardName());
		});
}

void MainWindow_SongsMode::on_btnInsertSlideSeparator_clicked()
{
	if(!isSongEditMode_)
		return;

	ui->teContent->setFocus();
	ui->teContent->insertPlainText("{---}");
}
