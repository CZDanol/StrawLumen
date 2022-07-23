#include "lumenexportdialog.h"
#include "ui_lumenexportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "gui/splashscreen.h"
#include "util/standarddialogs.h"
#include "importexport/exportdb.h"
#include "job/db.h"
#include "job/settings.h"

LumenExportDialog *lumenExportDialog()
{
	static LumenExportDialog *dlg = nullptr;
	if(!dlg)
		dlg = new LumenExportDialog(mainWindow);

	return dlg;
}

LumenExportDialog::LumenExportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LumenExportDialog)
{
	ui->setupUi(this);
}

LumenExportDialog::~LumenExportDialog()
{
	delete ui;
}

void LumenExportDialog::setSelectedSongs(const QVector<qlonglong> &songIds)
{
	ui->wgtSongSelection->setSelectedSongs(songIds);
}

void LumenExportDialog::on_btnClose_clicked()
{
	reject();
}

void LumenExportDialog::on_btnExport_clicked()
{
	QVector<qlonglong> songIds = ui->wgtSongSelection->selectedSongs();
	if(songIds.isEmpty())
		return standardErrorDialog(tr("Není vybrána žádná píseň k exportu."));

	QString outputFilePath_;
	{
		static const QIcon icon(":/icons/16/Export_16px.png");

		QFileDialog dlg(this);
		dlg.setFileMode(QFileDialog::AnyFile);
		dlg.setAcceptMode(QFileDialog::AcceptSave);
		dlg.setNameFilter(tr("Soubory Straw Lumen (*.strawLumen)"));
		dlg.setWindowIcon(icon);
		dlg.setWindowTitle(tr("Export"));
		dlg.setDirectory(settings->value("lumenExportDirectory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

		if(!dlg.exec())
			return;

		settings->setValue("lumenExportDirectory", dlg.directory().absolutePath());
		outputFilePath_ = dlg.selectedFiles().first();
	}

	bool isError = false;

	splashscreen->asyncAction(tr("Exportování písní"), true, [&]{
		ExportDatabaseManager exportDb(outputFilePath_, true);
		if(!exportDb.database().isOpen())
			return;

		connect(&exportDb, &ExportDatabaseManager::sigQueryError, [&]{isError = true;});

		exportDb.beginTransaction();

		// #: SONGS_TABLE_FIELDS
		for(int i = 0; i < songIds.size(); i ++) {
			splashscreen->setProgress(i, songIds.size());
			if(isError || splashscreen->isStornoPressed())
				break;

			const qlonglong songId = songIds[i];

			static const QStringList fields {
				"uid", "name", "author", "copyright", "content", "slideOrder", "notes", "lastEdit", "standardized_name"
			};
			const QSqlRecord r = db->selectRow(QString("SELECT %1 FROM songs WHERE id = ?").arg(fields.join(',')), {songId});

			QHash<QString,QVariant> data;
			for(const QString &field : fields)
				data.insert(field, r.value(field));

			const qlonglong exportSongId = exportDb.insert("songs", data).toLongLong();

			QSqlQuery q = db->selectQuery("SELECT tag FROM song_tags WHERE song = ?", {songId});
			while(q.next())
				exportDb.exec("INSERT INTO song_tags(song, tag) VALUES(?, ?)", {exportSongId, q.value("tag")});
		}

		exportDb.commitTransaction();
	});

	if(!isError && !splashscreen->isStornoPressed())
		standardSuccessDialog(tr("Vybrané písně byly exportovány do \"%1\".").arg(outputFilePath_));
}
