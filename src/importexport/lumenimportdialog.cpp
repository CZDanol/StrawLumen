#include "lumenimportdialog.h"
#include "ui_lumenimportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QFileInfo>
#include <QDebug>

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "gui/splashscreen.h"
#include "job/db.h"
#include "job/settings.h"
#include "rec/playlist.h"
#include "presentation/native/presentation_song.h"
#include "importexport/exportdb.h"
#include "util/standarddialogs.h"

enum ConflictBehavior {
	cbSkip,
	cbKeepBoth,
	cbOverwrite
};

LumenImportDialog *lumenImportDialog()
{
	static LumenImportDialog *dlg = nullptr;
	if(!dlg)
		dlg = new LumenImportDialog(mainWindow);

	return dlg;
}

LumenImportDialog::LumenImportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LumenImportDialog)
{
	ui->setupUi(this);
}

LumenImportDialog::~LumenImportDialog()
{
	delete ui;
}

void LumenImportDialog::show()
{
	importFilename_.clear();
	updateUi();
	QDialog::show();
}

void LumenImportDialog::show(const QString &filename)
{
	importFilename_ = filename;
	updateUi();
	QDialog::show();
}

void LumenImportDialog::updateUi()
{
	ui->btnSelectFile->setText(importFilename_.isEmpty() ? tr("Vybrat soubor...") : QFileInfo(importFilename_).fileName());
	ui->btnImport->setEnabled(!importFilename_.isEmpty());
}

void LumenImportDialog::on_btnClose_clicked()
{
	reject();
}

void LumenImportDialog::on_btnImport_clicked()
{
	const int conflictBehavior = ui->cmbConflictBehavior->currentIndex();
	const bool addToPlaylist = ui->cbAddToPlaylist->isChecked();
	const QSet<QString> tags = ui->lnTags->toTags();

	bool isError = false;
	QVector<QSharedPointer<Presentation> > presentations;

	splashscreen->asyncAction(tr("Impportování písní"), true, [&]{
		ExportDatabaseManager importDb(importFilename_, false);
		if(!importDb.database().isOpen())
			return;

		connect(&importDb, &ExportDatabaseManager::sigQueryError, [&]{isError = true;});

		db->beginTransaction();

		// #: SONGS_TABLE_FIELDS
		QSqlQuery q = importDb.selectQuery("SELECT * FROM songs");
		while(q.next()) {
			if(isError || splashscreen->isStornoPressed())
				break;

			const QSqlRecord existingSong = db->selectRowDef("SELECT id, lastEdit FROM songs WHERE name = ?", {q.value("name")});
			qlonglong songId;

			static const QStringList dataFields {"name", "standardized_name", "author", "copyright", "content", "slideOrder", "notes", "lastEdit"};
			bool updateData = true;

			QHash<QString,QVariant> data;
			for(const QString &field : dataFields)
				data.insert(field, q.value(field));

			data["standardized_name"] = standardizeSongName(data["name"].toString());

			if(!existingSong.isEmpty() && conflictBehavior == cbSkip) {
				 songId = existingSong.value("id").toLongLong();
				 updateData = false;

			} else if(!existingSong.isEmpty() && conflictBehavior == cbOverwrite) {
				songId = existingSong.value("id").toLongLong();
				db->update("songs", data, "id = ?", {songId});

			} else {
				data["uid"] = q.value("uid");
				songId = db->insert("songs", data).toLongLong();
			}

			if(updateData) {
				db->updateSongFulltextIndex(songId);

				// Update tags
				db->exec("DELETE FROM song_tags WHERE song = ?", {songId});

				QSqlQuery q2 = importDb.selectQuery("SELECT tag FROM song_tags WHERE song = ?", {q.value("id")});
				while(q2.next())
					db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, q2.value(0)});

				for(const QString &tag : tags)
					db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, tag});
			}

			if(addToPlaylist)
				presentations.append(Presentation_Song::createFromDb(songId));
		}

		db->commitTransaction();
	});

	emit db->sigSongListChanged();

	if(!presentations.isEmpty() && (
			 presentations.size() < 20
			 || standardConfirmDialog(tr("Importovaných písní je mnoho (%1). Opravdu je chcete všechny přidat do programu promítání?").arg(presentations.size()))
			 )) {
			mainWindow->presentationMode()->playlist()->addItems(presentations);
	}

	if(!isError) {
		if(!splashscreen->isStornoPressed())
			standardSuccessDialog(tr("Písně byly importovány."));

		close();
	}
}

void LumenImportDialog::on_btnSelectFile_clicked()
{
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

	updateUi();
}
