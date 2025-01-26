#include "lumenimportdialog.h"
#include "ui_lumenimportdialog.h"

#include <QDate>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QSet>
#include <QSqlQuery>
#include <QStandardPaths>

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "gui/splashscreen.h"
#include "importexport/exportdb.h"
#include "job/db.h"
#include "job/settings.h"
#include "presentation/native/presentation_song.h"
#include "rec/playlist.h"
#include "util/standarddialogs.h"

// settingName, uiControl
#define LUMENIMPORT_SETTINGS_FACTORY(F) \
	F(lumenImport_stripTags, cbStripTags) \
	F(lumenImport_addDateLabel, cbAddDateLabel)

enum ConflictBehavior {
	cbSkip,
	cbKeepBoth,
	cbOverwrite
};

LumenImportDialog *lumenImportDialog() {
	static LumenImportDialog *dlg = nullptr;
	if(!dlg)
		dlg = new LumenImportDialog(mainWindow);

	return dlg;
}

LumenImportDialog::LumenImportDialog(QWidget *parent) : QDialog(parent),
																												ui(new Ui::LumenImportDialog) {
	ui->setupUi(this);

	ui->wgtSongSelection->setDb(nullptr, false);

	LUMENIMPORT_SETTINGS_FACTORY(SETTINGS_LINK);
	ui->lblCurrentDateLabel->setText(QDate::currentDate().toString("yyyy_MM_dd"));
}

LumenImportDialog::~LumenImportDialog() {
	delete ui;
}

void LumenImportDialog::show() {
	importFilename_.clear();
	showImpl();
}

void LumenImportDialog::show(const QString &filename) {
	importFilename_ = filename;
	showImpl();
}

void LumenImportDialog::closeEvent(QCloseEvent *)
{
	saveSetting("lumenImport.conflictBehavior", ui->cmbConflictBehavior);
	saveSetting("lumenImport.stripTags", ui->cbStripTags);
	saveSetting("lumenImport.cbAddDateLabel", ui->cbAddDateLabel);
	saveSetting("lumenImport.requireCategory", ui->cbRequireCategory);
}

void LumenImportDialog::showImpl()
{
	loadSetting("lumenImport.conflictBehavior", ui->cmbConflictBehavior);
	loadSetting("lumenImport.stripTags", ui->cbStripTags);
	loadSetting("lumenImport.cbAddDateLabel", ui->cbAddDateLabel);
	loadSetting("lumenImport.requireCategory", ui->cbRequireCategory);
	ui->lnCategories->clear();

	loadImportFile();
	updateUi();
	QDialog::show();
}

void LumenImportDialog::updateUi() {
	ui->btnSelectFile->setText(importFilename_.isEmpty() ? tr("Vybrat soubor...") : QFileInfo(importFilename_).fileName());
	ui->btnImport->setEnabled(!importFilename_.isEmpty());
}

void LumenImportDialog::loadImportFile() {
	if(importFilename_.isEmpty()) {
		db_.reset();
		ui->wgtSongSelection->clearSelection();
		ui->wgtSongSelection->setDb(nullptr, false);
		return;
	}

	db_.reset(new ExportDatabaseManager(importFilename_, false));
	if(db_->database().isOpen()) {
		ui->wgtSongSelection->setDb(db_.data(), false);
	}
	else {
		db_.reset();
		importFilename_.clear();
	}
}

void LumenImportDialog::on_btnClose_clicked() {
	reject();
}

void LumenImportDialog::on_btnImport_clicked() {
	QSet<QString> categories = ui->lnCategories->toTags();
	const bool useCategories = !categories.isEmpty();
	if(!useCategories && ui->cbRequireCategory->isChecked())
		return standardErrorDialog(tr("Je třeba zvolit kategorii pro import nebo vypnout vyžadování kategorií."), this);

	if(!ui->wgtSongSelection->isAnySongSelected())
		return standardErrorDialog(tr("Není vybrána žádná píseň pro import."), this);

	const int conflictBehavior = ui->cmbConflictBehavior->currentIndex();
	const bool addToPlaylist = ui->cbAddToPlaylist->isChecked();
	const bool stripTags = ui->cbStripTags->isChecked();

	QSet<int> ids;
	for(const int id: ui->wgtSongSelection->selectedSongs())
		ids += id;

	QSet<QString> tags = ui->lnTags->toTags();
	if(ui->cbAddDateLabel->isChecked())
		tags += ui->lblCurrentDateLabel->text();
	tags += categories;

	bool isError = false;
	QVector<QSharedPointer<Presentation>> presentations;

	splashscreen->asyncAction(tr("Importování písní"), true, [&] {
		ExportDatabaseManager importDb(importFilename_, false);
		if(!importDb.database().isOpen())
			return;

		connect(&importDb, &ExportDatabaseManager::sigQueryError, [&] { isError = true; });

		db->beginTransaction();

		// #: SONGS_TABLE_FIELDS
		QSqlQuery q = importDb.selectQuery("SELECT * FROM songs");
		while(q.next()) {
			if(isError || splashscreen->isStornoPressed())
				break;

			if(!ids.contains(q.value("id").toInt()))
				continue;

			QSqlRecord existingSong = db->selectRowDef("SELECT id, lastEdit, locked FROM songs WHERE name = ?", {q.value("name")});
			const auto existingSongId = existingSong.value("id").toLongLong();
			qlonglong songId;

			if(useCategories && !existingSong.isEmpty()) {
				bool categoriesIntersect = false;
				QSqlQuery q2 = db->selectQuery("SELECT tag FROM song_tags WHERE song = ?", {existingSongId});
				while(q2.next()) {
					categoriesIntersect |= categories.contains(q2.value("tag").toString());
				}
				if(!categoriesIntersect) {
					existingSong = {};
				}
			}

			static const QStringList dataFields{
				"name",
				"author",
				"copyright",
				"content",
				"slideOrder",
				"notes",
				"lastEdit",
				"locked",
			};
			bool updateData = true;

			QHash<QString, QVariant> data;
			for(const QString &field: dataFields)
				data.insert(field, q.value(field));

			data["standardized_name"] = standardizeSongName(data["name"].toString());

			if(!existingSong.isEmpty() && conflictBehavior == cbSkip) {
				songId = existingSongId;
				updateData = false;
			}
			else if(!existingSong.isEmpty() && conflictBehavior == cbOverwrite && !existingSong.value("locked").toBool()) {
				songId = existingSongId;
				db->update("songs", data, "id = ?", {songId});
			}
			else {
				data["uid"] = q.value("uid");
				songId = db->insert("songs", data).toLongLong();
			}

			if(updateData) {
				DatabaseManager::updateSongFulltextIndex(db, songId);

				// Update tags
				db->exec("DELETE FROM song_tags WHERE song = ?", {songId});

				if(!stripTags) {
					QSqlQuery q2 = importDb.selectQuery("SELECT tag FROM song_tags WHERE song = ?", {q.value("id")});
					while(q2.next())
						db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, q2.value(0)});
				}

				for(const QString &tag: std::as_const(tags))
					db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, tag});
			}

			if(addToPlaylist)
				presentations.append(Presentation_Song::createFromDb(songId));
		}

		db->commitTransaction();
	});

	emit db->sigSongListChanged();

	if(!presentations.isEmpty() && (presentations.size() < 20 || standardConfirmDialog(tr("Importovaných písní je mnoho (%1). Opravdu je chcete všechny přidat do programu promítání?").arg(presentations.size())))) {
		mainWindow->presentationMode()->playlist()->addItems(presentations);
	}

	if(!isError) {
		if(!splashscreen->isStornoPressed())
			standardSuccessDialog(tr("Písně byly importovány."));

		close();
	}
}

void LumenImportDialog::on_btnSelectFile_clicked() {
	static const QIcon icon(":/icons/16/Import_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Soubory Straw Lumen (*.strawLumen)"));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import"));
	dlg.setDirectory(settings->value("lumenExportDirectory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("lumenExportDirectory", dlg.directory().absolutePath());
	importFilename_ = dlg.selectedFiles().first();

	loadImportFile();
	updateUi();
}
