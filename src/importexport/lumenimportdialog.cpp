#include "lumenimportdialog.h"
#include "ui_lumenimportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QSqlQuery>

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

LumenImportDialog *lumenImportDialog()
{
	static LumenImportDialog *dlg = nullptr;
	if(!dlg)
		dlg = new LumenImportDialog(mainWindow);

	return dlg;
}

void LumenImportDialog::on_btnClose_clicked()
{
	reject();
}

void LumenImportDialog::on_btnImport_clicked()
{
	QString dbFilePath_;
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
		dbFilePath_ = dlg.selectedFiles().first();
	}

	const int conflictBehavior = ui->cmbConflictBehavior->currentIndex();
	const bool addToPlaylist = ui->cbAddToPlaylist->isChecked();

	bool isError = false;
	QList<QSharedPointer<Presentation>> presentations;

	splashscreen->asyncAction(tr("Impportování písní"), false, [&]{
		ExportDatabaseManager importDb(dbFilePath_, false);
		if(!importDb.database().isOpen())
			return;

		connect(&importDb, &ExportDatabaseManager::sigQueryError, [&]{isError = true;});

		db->beginTransaction();

		// #: SONGS_TABLE_FIELDS
		QSqlQuery q = importDb.selectQuery("SELECT * FROM songs");
		while(q.next()) {
			if(isError)
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

				QVariantList args;
				for(const QString &field : dataFields)
					args.append(q.value(field));

				args.append(songId);

				db->exec("UPDATE songs SET " + dataFields.join(" = ?, ") + " = ? WHERE id = ?", args);

			} else {
				QVariantList args;
				args.append(q.value("uid"));

				for(const QString &field : dataFields)
					args.append(q.value(field));

				songId = db->insert("INSERT INTO songs(uid, " + dataFields.join(", ") +") VALUES(?" + QString(", ?").repeated(dataFields.size()) + ")", args).toLongLong();
			}

			if(updateData) {
				db->updateSongFulltextIndex(songId);

				// Update tags
				db->exec("DELETE FROM song_tags WHERE song = ?", {songId});

				QSqlQuery q = importDb.selectQuery("SELECT tag FROM song_tags WHERE song = ?", {songId});
				while(q.next())
					db->exec("INSERT INTO song_tags(song, tag) VALUES(?, ?)", {songId, q.value("tag")});
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

	if(!isError)
		standardInfoDialog(tr("Písně byly importovány."));
}
