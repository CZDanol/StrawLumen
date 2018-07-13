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
#include <QDomDocument>
#include <QFile>

#include "gui/splashscreen.h"
#include "importexport/documentgenerationdialog.h"
#include "importexport/lumenexportdialog.h"
#include "importexport/lumenimportdialog.h"
#include "util/standarddialogs.h"
#include "util/scopeexit.h"
#include "rec/chord.h"
#include "job/db.h"
#include "job/settings.h"

// F(uiControl)
#define SONG_FIELDS_FACTORY(F) \
	F(lnName) F(lnAuthor) F(lnSlideOrder) F(teContent) F(lnCopyright) F(lnTags)

MainWindow_SongsMode::MainWindow_SongsMode(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow_SongsMode)
{
	ui->setupUi(this);
	ui->twSong->setCornerWidget(ui->twSongCorner);
	ui->twSongs->setCornerWidget(ui->twSongsCorner);
	ui->twTranspose->setCornerWidget(ui->twTransposeCorner);

	// Song list
	{
		connect(ui->wgtSongList, SIGNAL(sigCurrentChanged(qlonglong,int)), this, SLOT(onCurrentSongChanged(qlonglong, int)));
		connect(ui->wgtSongList, SIGNAL(sigSelectionChanged()), this, SLOT(onSelectionChanged()));
		connect(ui->wgtSongList, SIGNAL(sigItemActivated(qlonglong)), ui->btnEdit, SLOT(click()));
		connect(ui->wgtSongList, SIGNAL(sigCustomContextMenuRequested(QPoint)), this, SLOT(onSongListContextMenuRequested(QPoint)));

		ui->wgtSongList->setDragEnabled(false);

		songListContextMenu_.addAction(ui->actionDeleteSongs);
		songListContextMenu_.addAction(ui->actionCreateSongbookFromSelection);
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

	// Tags
	{
		tagsCompleter_ = new QCompleter(this);
		tagsCompleter_->setModel(&tagsCompleterModel_);
		tagsCompleter_->setCaseSensitivity(Qt::CaseInsensitive);

		ui->lnTags->setCompleter(tagsCompleter_);
		ui->lnTags->setCompleterSuffix(", ");
	}

	// Insert song section menu
	{
		const QStringList sectionNames {
			"C", "V1", "V2", "V3", "B", "I", "O"
		};

		for(auto &sectionName : sectionNames) {
			SongSection section(sectionName);
			insertSectionMenu_.addAction(section.icon(), section.userFriendlyName(), [=]{
				insertSongSection(section);
			});
		}

		SongSection customSection = SongSection::customSection("");
		insertSectionMenu_.addAction(customSection.icon(), tr("Vlastní název"), [=]{
			insertSongSection(customSection);
		});

		ui->btnInsertSection->setMenu(&insertSectionMenu_);
	}

	// Import & export menus
	{
		importMenu_.addAction(ui->actionImportFromLumen);
		importMenu_.addAction(ui->actionImportOpenSongSong);

		exportMenu_.addAction(ui->actionExportToLumen);

		ui->btnImport->setMenu(&importMenu_);
		ui->btnExport->setMenu(&exportMenu_);
	}

	// Shortcuts
	{
		new QShortcut(Qt::CTRL | Qt::Key_Return, ui->btnSaveChanges, SLOT(click()));
		new QShortcut(Qt::CTRL | Qt::Key_S, ui->btnSaveChanges, SLOT(click()));
		new QShortcut(Qt::CTRL | Qt::Key_N, ui->btnNew, SLOT(click()));
		new QShortcut(Qt::Key_Escape, ui->btnDiscardChanges, SLOT(click()));
		new QShortcut(Qt::Key_F2, ui->btnEdit, SLOT(click()));

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
	ui->btnImport->setEnabled(!set);
	ui->btnSaveChanges->setVisible(set);
	ui->btnDiscardChanges->setVisible(set);

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

	// TODO: maybe add close button?
	return true;
}

void MainWindow_SongsMode::updateSongManipulationButtonsEnabled()
{
	ui->btnEdit->setEnabled(!isSongEditMode_ && currentSongId_ != -1);
	ui->actionDeleteSongs->setEnabled(ui->wgtSongList->selectedRowCount() > 0);
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

	QString tags;
	QSqlQuery q = db->selectQuery("SELECT tag FROM song_tags WHERE song = ? ORDER BY tag ASC", {currentSongId_});
	while(q.next()) {
		if(!tags.isEmpty())
			tags.append(", ");

		tags.append(q.value("tag").toString());
	}
	ui->lnTags->setText(tags);
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
		{"slideOrder", ui->lnSlideOrder->text()},
		{"lastEdit", QDateTime::currentSecsSinceEpoch()}
	};

	if(currentSongId_ == -1) {
		QVariantList args{
			QUuid::createUuid().toString()
		};
		args.append(data.values());
		currentSongId_ = db->insert("INSERT INTO songs(uid, " + QStringList(data.keys()).join(", ") + ") VALUES(?" + QString(", ?").repeated(data.size()) + ")", args).toLongLong();

	} else {
		QList<QVariant> args;
		args.append(data.values());
		args.append(currentSongId_);

		db->exec("UPDATE songs SET " + QStringList(data.keys()).join("= ?, ") + " = ? WHERE id = ?", args);
	}

	db->updateSongFulltextIndex(currentSongId_);

	// Tags
	db->exec("DELETE FROM song_tags WHERE song = ?", {currentSongId_});
	QSet<QString> tags;
	for(QString tag : ui->lnTags->text().toLower().split(',')) {
		tag = tag.trimmed();

		if(tag.isEmpty() || tags.contains(tag))
			continue;

		tags.insert(tag);
		db->exec("INSERT INTO song_tags(song, tag) VALUES(?, ?)", {currentSongId_, tag});
	}

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
	QVector<qlonglong> selectedIds = ui->wgtSongList->selectedRowIds();

	if(selectedIds.isEmpty())
		return;

	if(isSongEditMode_)
		return standardErrorDialog(tr("V režimu úprav nelze mazat písně. Ukončete úpravy a zkuste to znovu."));

	if(!standardDeleteConfirmDialog(tr("Opravdu smazat vybrané písně?")))
		return;

	db->beginTransaction();
	for(qlonglong id : selectedIds) {
		db->exec("DELETE FROM songs_fulltext WHERE docid = ?", {id});
		db->exec("DELETE FROM songs WHERE id = ?", {id});
	}
	db->commitTransaction();

	const bool prevBlocked = db->blockListChangedSignals(true);

	for(qlonglong id : selectedIds)
		emit db->sigSongChanged(id);

	db->blockListChangedSignals(prevBlocked);
	emit db->sigSongListChanged();

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
	transposeSong(song, 1, ui->btnTransposeFlat->isChecked());
	ui->teContent->setPlainText(song);
}

void MainWindow_SongsMode::on_btnTransposeDown_clicked()
{
	QString song = ui->teContent->toPlainText();
	transposeSong(song, -1, ui->btnTransposeFlat->isChecked());
	ui->teContent->setPlainText(song);
}

void MainWindow_SongsMode::on_btnAddCustomSlideOrderItem_pressed()
{
	addCustomSlideOrderItemMenu_.clear();

	for(SongSection section : songSections(ui->teContent->toPlainText()))
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
	static const QPixmap icon(":/icons/16/OpenSong_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Písně OpenSongu (*)"));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import OpenSong písní"));
	dlg.setDirectory(settings->value("dialog.importOpenSong.directory", QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("dialog.importOpenSong.directory", dlg.directory().absolutePath());

	splashscreen->asyncAction(tr("Import písní"), true, [&]{
		db->beginTransaction();
		SCOPE_EXIT(db->commitTransaction());

		const QStringList files = dlg.selectedFiles();
		for(int i = 0; i < files.size(); i++) {
			const QString filename = files[i];

			if(splashscreen->isStornoPressed())
				return;

			splashscreen->setProgress(i, files.size());

			QFile f(filename);
			if(!f.open(QIODevice::ReadOnly))
				return standardErrorDialog(tr("Nepodařilo se otevřít soubor \"%1\".").arg(filename));

			QDomDocument dom;
			if(!dom.setContent(&f))
				return standardErrorDialog(tr("Nepodařilo se načíst soubor \"%1\".").arg(filename));

			f.close();

			QDomElement root = dom.documentElement();
			if(root.tagName() != "song")
				return standardErrorDialog(tr("Soubor \"%1\" není formátu OpenSong.").arg(filename));

			// #: SONGS_TABLE_FIELDS
			const QString name = root.firstChildElement("title").text();
			const QString author = root.firstChildElement("author").text();
			const QString copyright = root.firstChildElement("copyright").text();
			const QString slideOrder = root.firstChildElement("presentation").text();

			QString content = root.firstChildElement("lyrics").text().trimmed();

			// Change sections formet
			static const QRegularExpression sectionRegex("\\[([VCPBTIOSNR][0-9]*)\\]\\s*");
			content.replace(sectionRegex, "{\\1}\n");

			// Change chords format
			static const QRegularExpression chordLineRegex("^(\\.[^\n]*\n)([^\n]*)$", QRegularExpression::MultilineOption);
			int offsetCorrection = 0;
			QRegularExpressionMatchIterator it = chordLineRegex.globalMatch(content);
			while(it.hasNext()) {
				const QRegularExpressionMatch m = it.next();

				content.remove(m.capturedStart(1)+offsetCorrection, m.capturedLength(1));
				offsetCorrection -= m.capturedLength(1);

				static const QRegularExpression chordRegex("\\S+");
				QRegularExpressionMatchIterator it2 = chordRegex.globalMatch(m.captured(1), 1);
				while(it2.hasNext()) {
					const QRegularExpressionMatch m2 = it2.next();
					const QString insertText = QString("[%1]").arg(m2.captured());
					content.insert(m.capturedStart(2)+m2.capturedStart()+offsetCorrection, insertText);
					offsetCorrection += insertText.length();
				}
			}

			// Trim spaces on line beginnings and ends
			static const QRegularExpression trimmingRegex("^[ \t]+|[ \t]+$", QRegularExpression::MultilineOption);
			content.remove(trimmingRegex);

			const QVariant id = db->insert("INSERT INTO songs(uid, name, author, copyright, content, slideOrder, lastEdit) VALUES(?, ?, ?, ?, ?, ?, ?)", {
									 QUuid::createUuid().toString(),
									 name, author, copyright, content, slideOrder,
									 QDateTime::currentSecsSinceEpoch(),
								 });

			db->updateSongFulltextIndex(id.toLongLong());
		}
	});

	emit db->sigSongListChanged();
}

void MainWindow_SongsMode::on_lnTags_sigFocused()
{
	tagsCompleterModel_.setQuery(db->selectQuery("SELECT DISTINCT tag FROM song_tags ORDER BY tag ASC"));
}

void MainWindow_SongsMode::on_btnCreateSongbook_clicked()
{
	documentGenerationDialog()->show();
	documentGenerationDialog()->setSelectedSongs(ui->wgtSongList->selectedRowIds());
}

void MainWindow_SongsMode::on_actionCreateSongbookFromSelection_triggered()
{
	documentGenerationDialog()->show();
	documentGenerationDialog()->setSelectedSongs(ui->wgtSongList->selectedRowIds());
}

void MainWindow_SongsMode::on_actionExportToLumen_triggered()
{
	lumenExportDialog()->show();
	lumenExportDialog()->setSelectedSongs(ui->wgtSongList->selectedRowIds());
}

void MainWindow_SongsMode::on_actionImportFromLumen_triggered()
{
	lumenImportDialog()->show();
}
