#include "lumenimportdialog.h"
#include "ui_lumenimportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QFileInfo>

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
	cbOverwriteIfNewer,
	cbAlwaysOverwrite
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

			const QSqlRecord existingSong = db->selectRowDef("SELECT id, lastEdit FROM songs WHERE uid = ?", {q.value("uid")});
			qlonglong songId;

			static const QStringList dataFields {"name", "author", "copyright", "content", "slideOrder", "lastEdit"};
			bool updateData = true;

			if(!existingSong.isEmpty() && (conflictBehavior == cbSkip || (conflictBehavior == cbOverwriteIfNewer && existingSong.value("lastEdit").toLongLong() >= q.value("lastEdit").toLongLong()))) {
				 songId = existingSong.value("id").toLongLong();
				 updateData = false;

			} else if(!existingSong.isEmpty()) {
				songId = existingSong.value("id").toLongLong();

				QHash<QString,QVariant> data;
				for(const QString &field : dataFields)
					data.insert(field, q.value(field));

				db->update("songs", data, "id = ?", {songId});

			} else {
				QHash<QString,QVariant> data;
				for(const QString &field : dataFields)
					data.insert(field, q.value(field));

				data.insert("uid", q.value("uid"));

				songId = db->insert("songs", data).toLongLong();
			}

			if(updateData) {
				db->updateSongFulltextIndex(songId);

				// Update tags
				db->exec("DELETE FROM song_tags WHERE song = ?", {songId});

				QSqlQuery q = importDb.selectQuery("SELECT tag FROM song_tags WHERE song = ?", {songId});
				while(q.next())
					db->exec("INSERT INTO song_tags(song, tag) VALUES(?, ?)", {songId, q.value("tag")});
			}

			for(const QString &tag : tags)
				db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, tag});

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
