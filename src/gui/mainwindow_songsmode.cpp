#include "mainwindow_songsmode.h"
#include "ui_mainwindow_songsmode.h"

#include <QStyle>
#include <QUuid>
#include <QShortcut>
#include <QMenu>
#include <QStringList>
#include <QDateTime>
#include <QFileDialog>
#include <QStandardPaths>
#include <QClipboard>
#include <QMimeData>

#include "gui/splashscreen.h"
#include "gui/mainwindow_presentationmode.h"
#include "gui/bulkeditsongsdialog.h"
#include "presentation/native/presentation_song.h"
#include "importexport/documentgenerationdialog.h"
#include "importexport/lumenexportdialog.h"
#include "importexport/lumenimportdialog.h"
#include "importexport/opensongimportdialog.h"
#include "importexport/opensongexportdialog.h"
#include "util/standarddialogs.h"
#include "util/scopeexit.h"
#include "util/execonmainthread.h"
#include "util/regex.h"
#include "util/songcontentsyntaxhiglighter.h"
#include "rec/chord.h"
#include "rec/playlist.h"
#include "job/db.h"
#include "job/settings.h"
#include "job/wordsplit.h"
#include "presentation/presentationmanager.h"

// F(uiControl)
#define SONG_FIELDS_FACTORY(F) \
	F(lnName) F(lnAuthor) F(lnSlideOrder) F(teContent) F(lnCopyright) F(lnTags) F(teNotes)

MainWindow_SongsMode::MainWindow_SongsMode(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow_SongsMode)
{
	ui->setupUi(this);
	ui->twSong->setCornerWidget(ui->twSongCorner);
	ui->twSongs->setCornerWidget(ui->twSongsCorner);
	ui->twTranspose->setCornerWidget(ui->twTransposeCorner);

	ui->twSong->setCurrentIndex(0);

	// Song list
	{
		connect(ui->wgtSongList, SIGNAL(sigCurrentChanged(qlonglong,int)), this, SLOT(onCurrentSongChanged(qlonglong, int)));
		connect(ui->wgtSongList, SIGNAL(sigSelectionChanged()), this, SLOT(onSelectionChanged()));
		connect(ui->wgtSongList, SIGNAL(sigItemActivated(qlonglong)), ui->btnEdit, SLOT(click()));
		connect(ui->wgtSongList, SIGNAL(sigCustomContextMenuRequested(QPoint)), this, SLOT(onSongListContextMenuRequested(QPoint)));

		ui->wgtSongList->setDragEnabled(false);

		songListContextMenu_.addAction(ui->actionDeleteSongs);
		songListContextMenu_.addSeparator();
		songListContextMenu_.addAction(ui->actionPresentSongs);
		songListContextMenu_.addAction(ui->actionAddSongsToPlaylist);
		songListContextMenu_.addSeparator();
		songListContextMenu_.addAction(ui->actionCreateSongbookFromSelection);
		songListContextMenu_.addMenu(&exportMenu_);
	}

	// Slide order
	{
		slideOrderCompleter_ = new QCompleter(this);
		slideOrderCompleter_->setModel(&slideOrderCompleterModel_);
		slideOrderCompleter_->setCaseSensitivity(Qt::CaseInsensitive);

		slideOrderValidator_.setRegularExpression(songCustomSlideOrderRegex());

		ui->lnSlideOrder->setCompleter(slideOrderCompleter_);
		ui->lnSlideOrder->setValidator(&slideOrderValidator_);
		ui->lnSlideOrder->setCompleterSuffix(" ");

		ui->btnAddCustomSlideOrderItem->setMenu(&addCustomSlideOrderItemMenu_);
	}

	// Insert song section menu
	{
		const QStringList sectionNames {
			"C", "V1", "V2", "V3", "B", "I", "O", "M", "P"
		};

		for(auto &sectionName : sectionNames) {
			SongSection section(sectionName);
			insertSectionMenu_.addAction(section.icon(), section.userFriendlyName(), [=]{
				insertSongSection(section);
			});
		}

		SongSection customSection = SongSection::customSection("");
		insertSectionMenu_.addAction(customSection.icon(), tr("Vlastní název"), [=]{
			insertSongSection(customSection, true);
		});

		ui->btnInsertSection->setMenu(&insertSectionMenu_);
	}

	// Import & export menus
	{
		importMenu_.setTitle(tr("Import"));
		importMenu_.setIcon(QPixmap(":/icons/16/Import_16px.png"));
		importMenu_.addAction(ui->actionImportFromLumen);
		importMenu_.addAction(ui->actionImportOpenSongSong);

		exportMenu_.setTitle(tr("Export"));
		exportMenu_.setIcon(QPixmap(":/icons/16/Export_16px.png"));
		exportMenu_.addAction(ui->actionExportToLumen);
		exportMenu_.addAction(ui->actionExportToOpenSong);

		ui->btnImport->setMenu(&importMenu_);
		ui->btnExport->setMenu(&exportMenu_);
	}

	// Insert panel
	{
		copyChordsMenu_.setTitle(tr("Kopírovat akordy z..."));
		copyChordsMenu_.setIcon(QPixmap(":/icons/16/Music Notation_16px.png"));

		ui->btnCopyChords->setMenu(&copyChordsMenu_);
	}

	// Content text edit
	{
		connect(ui->teContent, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTeContentContextMenuRequested(QPoint)));
		connect(ui->teContent, &SongContentTextEdit::sigAltlLeftPressed, ui->actionMoveChordsLeft, &QAction::trigger);
		connect(ui->teContent, &SongContentTextEdit::sigAltRightPressed, ui->actionMoveChordsRight, &QAction::trigger);

		teContentMenu_.clear();
		teContentMenu_.addAction(ui->actionMoveChordsLeft);
		teContentMenu_.addAction(ui->actionMoveChordsRight);
		teContentMenu_.addSeparator();
		teContentMenu_.addAction(ui->actionDeleteChords);
		teContentMenu_.addAction(ui->actionOnlyChords);
		teContentMenu_.addMenu(&copyChordsMenu_);
	}

	// Shortcuts
	{
		new QShortcut(Qt::CTRL | Qt::Key_Return, ui->btnSaveChanges, SLOT(click()));
		new QShortcut(Qt::CTRL | Qt::Key_S, ui->btnSaveChanges, SLOT(click()));
		new QShortcut(Qt::CTRL | Qt::Key_N, ui->btnNew, SLOT(click()));
		new QShortcut(Qt::CTRL | Qt::Key_P, ui->btnCreateSongbook, SLOT(click()));
		new QShortcut(Qt::ALT | Qt::SHIFT | Qt::Key_F, ui->btnAutoFormat, SLOT(click()));
		new QShortcut(Qt::Key_Escape, ui->btnDiscardChanges, SLOT(click()));
		new QShortcut(Qt::Key_F2, ui->btnEdit, SLOT(click()));

		connect(new QShortcut(Qt::CTRL | Qt::Key_F12, ui->teContent), &QShortcut::activated, this, [this](){
			ui->teContent->syntaxHiglighter()->setSepSyllables(!ui->teContent->syntaxHiglighter()->sepSyllables());
		});

		connect(new QShortcut(Qt::Key_Delete, this, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), ui->actionDeleteSongs, SLOT(trigger()));
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

QMenu *MainWindow_SongsMode::importMenu()
{
	return &importMenu_;
}

QMenu *MainWindow_SongsMode::exportMenu()
{
	return &exportMenu_;
}

void MainWindow_SongsMode::editSong(qlonglong songId)
{
	onCurrentSongChanged(songId, ui->wgtSongList->currentRowIndex());
	ui->btnEdit->click();
}

void MainWindow_SongsMode::requestDeleteSongs(const QVector<qlonglong> &songIds)
{
	if(songIds.isEmpty())
		return;

	if(isSongEditMode_)
		return standardErrorDialog(tr("V režimu úprav nelze mazat písně. Ukončete úpravy a zkuste to znovu."));

	if(!standardDeleteConfirmDialog(tr("Opravdu smazat vybrané písně?")))
		return;

	db->beginTransaction();
	for(qlonglong id : songIds) {
		db->exec("DELETE FROM songs_fulltext WHERE docid = ?", {id});
		db->exec("DELETE FROM songs WHERE id = ?", {id});
		db->exec("DELETE FROM song_tags WHERE song = ?", {id});
	}
	db->commitTransaction();

	const bool prevBlocked = db->blockListChangedSignals(true);

	for(qlonglong id : songIds)
		emit db->sigSongChanged(id);

	db->blockListChangedSignals(prevBlocked);
	emit db->sigSongListChanged();

	currentSongId_ = -1;
	updateSongManipulationButtonsEnabled();
}

bool MainWindow_SongsMode::askFinishEditMode()
{
	if(!isSongEditMode_)
		return true;

	if(standardConfirmDialog(tr("Píseň je otevřena pro editaci. Chcete uložit provedené úpravy?")))
		ui->btnSaveChanges->click();
	else {
		setSongEditMode(false);
		fillSongData();
	}

	// TODO: maybe add cancel button?
	return true;
}

QVector<qlonglong> MainWindow_SongsMode::selectedSongIds()
{
	return ui->wgtSongList->selectedRowIds();
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
	ui->btnImport->setEnabled(!set);
	ui->btnSaveChanges->setVisible(set);
	ui->btnDiscardChanges->setVisible(set);

	ui->twInsert->setVisible(set);
	ui->twEdit->setVisible(set);
	ui->twTranspose->setVisible(set);
	ui->twImportExport->setVisible(!set);

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
	ui->btnEdit->setEnabled(!isSongEditMode_ && currentSongId_ != -1);
	ui->actionDeleteSongs->setEnabled(ui->wgtSongList->selectedRowCount() > 0);
	ui->actionPresentSongs->setEnabled(ui->wgtSongList->selectedRowCount() > 0);
}

void MainWindow_SongsMode::updateCopyChordsMenu()
{
	const auto sections = songSectionsWithContent(ui->teContent->toPlainText());

	QList<SongSectionWithContent> relevantSections;
	for(const SongSectionWithContent &sc : sections) {
		QVector<ChordInSong> chords;
		const QString sectionContentWithoutChords = removeSongChords(sc.content, chords).trimmed();

		if(sectionContentWithoutChords.isEmpty() || chords.isEmpty())
			continue;

		relevantSections += sc;
	}

	copyChordsMenu_.clear();

	if(QApplication::clipboard()->mimeData()->hasText()) {
		copyChordsMenu_.addAction(QIcon(":/icons/16/Paste_16px.png"), tr("Schránka"), [this]{
			contentSelectionMorph([](QString str){
				QVector<ChordInSong> chords;
				QString newStr = removeSongChords(str, chords);

				if(chords.size() && !standardConfirmDialog(tr("Sekce, do které by se měly akordy kopírovat, již akordy obsahuje. Chcete pokračovat a přepsat tyto akordy?")))
					 return str;

				return copySongChords(QApplication::clipboard()->text(), newStr);
			}, true);
		});
	}

	for(const SongSectionWithContent &sc : relevantSections) {
		const QString sourceContent = sc.content;
		copyChordsMenu_.addAction(sc.section.icon(), sc.section.userFriendlyName(), [this, sourceContent]{
			contentSelectionMorph([sourceContent](QString str){
				QVector<ChordInSong> chords;
				QString newStr = removeSongChords(str, chords);

				if(chords.size() && !standardConfirmDialog(tr("Sekce, do které by se měly akordy kopírovat, již akordy obsahuje. Chcete pokračovat a přepsat tyto akordy?")))
					 return str;

				return copySongChords(sourceContent, newStr);
			}, true);
		});
	}

	copyChordsMenu_.setEnabled(isSongEditMode_ && !copyChordsMenu_.actions().isEmpty());
}

void MainWindow_SongsMode::updateTeContentMenu()
{
	updateCopyChordsMenu();
	updateMoveChordActionsEnabled();

	teContentMenu_.setEnabled(isSongEditMode_);
}

void MainWindow_SongsMode::updateMoveChordActionsEnabled()
{
	const bool enabled = isSongEditMode_ && !chordsInsideSelection(ui->teContent->textCursor()).isEmpty();
	ui->actionMoveChordsLeft->setEnabled(enabled);
	ui->actionMoveChordsRight->setEnabled(enabled);
}

void MainWindow_SongsMode::insertSongSection(const SongSection &section, bool positionCursorInMiddle)
{
	if(!isSongEditMode_)
		return;

	if(ui->teContent->textCursor().positionInBlock() != 0)
		ui->teContent->insertPlainText("\n");

	ui->teContent->insertPlainText("\n" + section.annotation() + "\n");
	ui->teContent->setFocus();

	if(positionCursorInMiddle) {
		QTextCursor c = ui->teContent->textCursor();
		c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 1 + section.annotation().length()/2);
		ui->teContent->setTextCursor(c);
	}
}

void MainWindow_SongsMode::contentSelectionMorph(const std::function<QString (QString)> &callback, bool onlySection)
{
	if(!isSongEditMode_)
		return;

	QTextCursor cursor = ui->teContent->textCursor();

	const bool hasSelection = cursor.hasSelection();
	if(!hasSelection && onlySection) {
		const QPair<int, QString> section = songSectionAroundPos(cursor.position());

		if(section.first == -1)
			return standardInfoDialog(tr("Vybraná pozice neobsahuje žádnou sekci"));

		cursor.setPosition(section.first);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, section.second.length());

	} else if(!hasSelection)
		cursor.select(QTextCursor::Document);

	QString data = cursor.selectedText();
	data.replace(0x2029, '\n');
	data = callback(data);
	data.replace('\n', 0x2029);
	cursor.insertText(data);
	cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, data.length());

	if(hasSelection)
		ui->teContent->setTextCursor(cursor);
}

void MainWindow_SongsMode::moveChords(bool right)
{
	if(!isSongEditMode_)
		return;

	QString content = ui->teContent->toPlainText();
	ChordsInSong chords = chordsInsideSelection(ui->teContent->textCursor());

	if(right)
		std::reverse(chords.begin(), chords.end());

	if(chords.isEmpty())
		return;

	ChordsInSong _;
	QVector<int> splits = WordSplit::czech(content, _, WordSplit::IncludeNewlines | WordSplit::IncludeChords);
	int start = content.length(), end = 0;

	for(int chsI = 0; chsI < chords.length(); chsI++) {
		int annotPos = chords[chsI].annotationPos;
		int annotLength = chords[chsI].annotationLength;

		int i = splits.indexOf(annotPos);

		if(right) {
			// Aggregate all directly following chords
			while(chsI+1 < chords.length() && chords[chsI+1].annotationPos + chords[chsI+1].annotationLength == annotPos) {
				annotPos -= chords[chsI+1].annotationLength;
				annotLength += chords[chsI+1].annotationLength;
				chsI++;
			}

			while(splits[i] <= annotPos + annotLength)
				i++;

		} else {
			// Aggregate all directly following chords
			while(chsI+1 < chords.length() && chords[chsI+1].annotationPos == annotPos + annotLength) {
				annotLength += chords[chsI+1].annotationLength;
				chsI++;
			}

			i--;
		}

		if(i < 0 || i >= splits.length())
			continue;

		const QString movedStr = content.mid(annotPos, annotLength);

		if(right) {
			content.insert(splits[i], movedStr);
			content.remove(annotPos, annotLength);

			start = qMin(start, splits[i] - annotLength);
			end = qMax(end, splits[i]);
		} else {
			content.remove(annotPos, annotLength);
			content.insert(splits[i], movedStr);

			start = qMin(start, splits[i]);
			end = qMax(end, splits[i] + annotLength);
		}
	}

	// Keep the insertion point on minimum
	QTextCursor cursor(ui->teContent->document());
	cursor.setPosition(0);
	cursor.setPosition(content.length(), QTextCursor::KeepAnchor);
	cursor.insertText(content);

	cursor.setPosition(start);
	cursor.setPosition(end, QTextCursor::KeepAnchor);
	ui->teContent->setTextCursor(cursor);
}

QPair<int, QString> MainWindow_SongsMode::songSectionAroundPos(int pos)
{
	for(const SongSectionWithContent &sc : songSectionsWithContent(ui->teContent->toPlainText())) {
		if(pos >= sc.annotationPos && pos < sc.untrimmedContentEnd)
			return QPair<int, QString>(sc.contentPos, sc.content);
	}

	return QPair<int, QString>(-1, QString());
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
	ui->lnCopyright->setText(r.value("copyright").toString());
	ui->teNotes->setPlainText(r.value("notes").toString());

	QString tags;
	QSqlQuery q = db->selectQuery("SELECT tag FROM song_tags WHERE song = ? ORDER BY tag ASC", {currentSongId_});
	while(q.next()) {
		if(!tags.isEmpty())
			tags.append(", ");

		tags.append(q.value("tag").toString());
	}
	ui->lnTags->setText(tags);
}

void MainWindow_SongsMode::onCurrentSongChanged(qlonglong songId, int prevRowId)
{
	if(songId == currentSongId_)
		return;

	if(!askFinishEditMode()) {
		ui->wgtSongList->selectRow(prevRowId);
		return;
	}

	if(songId == -1)
		return;

	currentSongId_ = songId;
	setSongEditMode(false);
	fillSongData();
}

void MainWindow_SongsMode::onSelectionChanged()
{
	updateSongManipulationButtonsEnabled();
}

void MainWindow_SongsMode::onSongListContextMenuRequested(const QPoint &globalPos)
{
	songListContextMenu_.popup(globalPos);
}

void MainWindow_SongsMode::onTeContentContextMenuRequested(const QPoint &pos)
{
	auto cursor = ui->teContent->textCursor();
	const auto cursorForPosition = ui->teContent->cursorForPosition(pos);

	// If we right-clicked inside the selection, we use the selection. Otherwise, we use just the position
	if(cursorForPosition.position() < cursor.selectionStart() || cursorForPosition.position() >= cursor.selectionEnd())
		ui->teContent->setTextCursor(cursorForPosition);

	updateTeContentMenu();
	teContentMenu_.exec(ui->teContent->viewport()->mapToGlobal(pos));
}

void MainWindow_SongsMode::on_btnNew_clicked()
{
	ui->wgtSongList->unselect();

	ui->lnName->setText("Nová písnička");
	ui->lnAuthor->clear();
	ui->lnSlideOrder->clear();
	ui->teContent->clear();
	ui->teNotes->clear();
	ui->lnTags->setText(ui->wgtSongList->currentTagFilterName());

	currentSongId_ = -1;

	setSongEditMode(true);
	ui->lnName->setFocus();
	ui->lnName->selectAll();
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
	// #: SONGS_TABLE_FIELDS

	setSongEditMode(false);

	db->beginTransaction();

	QHash<QString,QVariant> data{
		{"name", ui->lnName->text()},
		{"author", ui->lnAuthor->text()},
		{"copyright", ui->lnCopyright->text()},
		{"content", ui->teContent->toPlainText()},
		{"slideOrder", ui->lnSlideOrder->text().simplified()},
		{"notes", ui->teNotes->toPlainText()},
		{"lastEdit", QDateTime::currentSecsSinceEpoch()}
	};

	if(currentSongId_ == -1) {
		data.insert("uid", QUuid::createUuid().toString());
		currentSongId_ = db->insert("songs", data).toLongLong();

	} else
		db->update("songs", data, "id = ?", {currentSongId_});

	db->updateSongFulltextIndex(currentSongId_);

	// Tags
	db->exec("DELETE FROM song_tags WHERE song = ?", {currentSongId_});
	for(QString tag : ui->lnTags->toTags())
		db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {currentSongId_, tag});

	db->commitTransaction();

	updateSongManipulationButtonsEnabled();
	emit db->sigSongChanged(currentSongId_);

}

void MainWindow_SongsMode::on_btnEdit_clicked()
{
	setSongEditMode(true);
	ui->lnName->setFocus();
}

void MainWindow_SongsMode::on_actionDeleteSongs_triggered()
{
	requestDeleteSongs(ui->wgtSongList->selectedRowIds());
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
	contentSelectionMorph([=](QString content) {
		transposeSong(content, 1, ui->btnTransposeFlat->isChecked());
		return content;
	}, false);
}

void MainWindow_SongsMode::on_btnTransposeDown_clicked()
{
	contentSelectionMorph([=](QString content) {
		transposeSong(content, -1, ui->btnTransposeFlat->isChecked());
		return content;
	}, false);
}

void MainWindow_SongsMode::on_btnAddCustomSlideOrderItem_pressed()
{
	addCustomSlideOrderItemMenu_.clear();

	const auto sections = songSections(ui->teContent->toPlainText());
	if(sections.isEmpty())
		return standardInfoDialog(tr("Píseň aktuálně neobsahuje žádné sekce."));

	for(SongSection section : sections)
		addCustomSlideOrderItemMenu_.addAction(section.icon(), section.userFriendlyName(), [this, section]{
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

void MainWindow_SongsMode::on_actionImportOpenSongSong_triggered()
{
	openSongImportDialog()->show();
}

void MainWindow_SongsMode::on_btnCreateSongbook_clicked()
{
	documentGenerationDialog()->show();
	documentGenerationDialog()->setSelectedSongs(mainWindow->selectedSongIds());
}

void MainWindow_SongsMode::on_actionCreateSongbookFromSelection_triggered()
{
	documentGenerationDialog()->show();
	documentGenerationDialog()->setSelectedSongs(ui->wgtSongList->selectedRowIds());
}

void MainWindow_SongsMode::on_actionExportToLumen_triggered()
{
	lumenExportDialog()->show();
	lumenExportDialog()->setSelectedSongs(mainWindow->selectedSongIds());
}

void MainWindow_SongsMode::on_actionImportFromLumen_triggered()
{
	lumenImportDialog()->show();
}

void MainWindow_SongsMode::on_actionPresentSongs_triggered()
{
	if(isSongEditMode_)
		return standardErrorDialog(tr("Tuto akci nelze provést během úprav písně. Ukončete úpravy a zkuste to znovu."));

	QVector<QSharedPointer<Presentation> > presentations;
	for(qlonglong songId : ui->wgtSongList->selectedRowIds()) {
		auto pres = Presentation_Song::createFromDb(songId);
		if(pres)
			presentations.append(pres);
	}

	if(!presentations.size())
		return;

	mainWindow->presentationMode()->playlist()->addItems(presentations);
	mainWindow->showPresentationMode();
	presentationManager->setSlide(presentations.first(), 0);
}

void MainWindow_SongsMode::on_actionAddSongsToPlaylist_triggered()
{
	QVector<QSharedPointer<Presentation> > presentations;
	for(qlonglong songId : ui->wgtSongList->selectedRowIds()) {
		auto pres = Presentation_Song::createFromDb(songId);
		if(pres)
			presentations.append(pres);
	}

	if(!presentations.size())
		return;

	mainWindow->presentationMode()->playlist()->addItems(presentations);
	mainWindow->blinkPresentationModeButton();
}

void MainWindow_SongsMode::on_actionExportToOpenSong_triggered()
{
	openSongExportDialog()->show();
	openSongExportDialog()->setSelectedSongs(mainWindow->selectedSongIds());
}

void MainWindow_SongsMode::on_btnCopyChords_pressed()
{
	updateCopyChordsMenu();

	if(copyChordsMenu_.actions().isEmpty())
		return standardErrorDialog(tr("Píseň neobsahuje žádné sekce s textem i akordy."));
}

void MainWindow_SongsMode::on_actionDeleteChords_triggered()
{
	contentSelectionMorph([](QString str){
		static const QRegularExpression rxTrim("\\u2029\\s+|\\s+\\u2029", QRegularExpression::MultilineOption);

		str = removeSongChords(str);
		str = str.trimmed();
		str.remove(rxTrim);
		return str;
	}, true);
}

void MainWindow_SongsMode::on_btnAutoFormat_clicked()
{
	contentSelectionMorph([=](QString content){
		// Standardize chords
		ChordsInSong chords = songChords(content);
		int correction = 0;
		for(const ChordInSong &chs : chords) {
			const QString newAnnotation = QString("[%1]").arg(chs.chord.toString(chs.chord.isFlat()));
			content.replace(chs.annotationPos + correction, chs.annotationLength, newAnnotation);
			correction += newAnnotation.length() - chs.annotationLength;
		}

		// Trim whitespaces
		static const QRegularExpression rxTrim("^[ \t]+|[ \t]+$", QRegularExpression::MultilineOption);
		content.remove(rxTrim);

		// Compact spaces
		static const QRegularExpression rxCompactSpaces("[ \t]+");
		content.replace(rxCompactSpaces, " ");

		// Space after rxSpaceAfterInterpunction
		static const QRegularExpression rxSpaceAfterInterpunction("([,.!?;:])(?=\\p{L})");
		content.replace(rxSpaceAfterInterpunction, "\\1 ");

		// Newline after section annotation
		static const QRegularExpression rxAnnotationNewline(QString("\\s*(%1)\\s*").arg(songSectionAnnotationRegex().pattern()), QRegularExpression::DotMatchesEverythingOption);
		content.replace(rxAnnotationNewline, "\n\n\\1\n");

		content = content.trimmed();

		return content;
	}, false);
}

void MainWindow_SongsMode::on_actionMoveChordsRight_triggered()
{
	moveChords(true);
}

void MainWindow_SongsMode::on_teContent_cursorPositionChanged()
{
	updateMoveChordActionsEnabled();
}

void MainWindow_SongsMode::on_actionMoveChordsLeft_triggered()
{
	moveChords(false);
}

void MainWindow_SongsMode::on_actionOnlyChords_triggered()
{
	contentSelectionMorph([](QString str){
		QString newContent;
		int prev = 0;
		for(const ChordInSong &chs : songChords(str)) {
				if(str.mid(prev, chs.annotationPos-prev).contains(QChar(0x2029)))
						newContent += QChar(0x2029);

				newContent += str.mid(chs.annotationPos, chs.annotationLength);
				prev = chs.annotationPos;
		}

		return newContent;
	}, true);
}

void MainWindow_SongsMode::on_btnBulkEdit_clicked()
{
	bulkEditSongsDialog()->show();
	bulkEditSongsDialog()->setSelectedSongs(mainWindow->selectedSongIds());
}

void MainWindow_SongsMode::on_btnConvertChords_clicked()
{
	contentSelectionMorph([=](QString content){
		static const QString chordRegexPattern =
				Chord::chordRegex().pattern()
				.replace(QRegularExpression("\\((?!\\?\\:)"), "(?:") // Replace capturing groups with non-capturing
				.remove('^').remove('$') // Make it a global match
				;

		static QRegularExpression globalChordRegex(chordRegexPattern);

		static const QRegularExpression regex(
					// First - chords - line
					"^(\\.?(?:(?!\\n)\\s)*(?:" + chordRegexPattern + "(?:(?!\\n)\\s)*)+)"
					+ "\n"
					// Second - text - line
					+ "(.*?)$",
					QRegularExpression::MultilineOption
					);

		QString result = content;
		int resultPosCorrection = 0;

		QRegularExpressionMatchIterator mi = regex.globalMatch(content);
		while(mi.hasNext()) {
			const QRegularExpressionMatch m = mi.next();

			const QString chordLine = m.captured(1);
			QString textLine = m.captured(2);
			int textLinePosCorrection = 0;

			QRegularExpressionMatchIterator mi2 = globalChordRegex.globalMatch(chordLine);
			while(mi2.hasNext()) {
				const QRegularExpressionMatch m2 = mi2.next();
				const QString chordTag = QStringLiteral("[%1]").arg(m2.captured());

				textLine.insert(m2.capturedStart() + textLinePosCorrection, chordTag);
				textLinePosCorrection += chordTag.length();
			}

			result.replace(m.capturedStart() + resultPosCorrection, m.capturedLength(), textLine);
			resultPosCorrection += textLine.length() - m.capturedLength();
		}

		return result;
	}, false);
}
