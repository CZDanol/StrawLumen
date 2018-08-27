#include "bulkeditsongsdialog.h"
#include "ui_bulkeditsongsdialog.h"

#include "gui/mainwindow.h"
#include "gui/splashscreen.h"
#include "job/db.h"

BulkEditSongsDialog::BulkEditSongsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::BulkEditSongsDialog)
{
	ui->setupUi(this);
}

BulkEditSongsDialog::~BulkEditSongsDialog()
{
	delete ui;
}

BulkEditSongsDialog *bulkEditSongsDialog()
{
	static BulkEditSongsDialog *result = new BulkEditSongsDialog(mainWindow);
	return result;
}

void BulkEditSongsDialog::on_btnStorno_clicked()
{
	reject();
}

void BulkEditSongsDialog::on_btnGenerate_clicked()
{
	splashscreen->asyncAction(tr("Provádění úprav"), false, [=](){
		db->beginTransaction();

		QString songIdList;
		for(const qlonglong id : ui->wgtSongSelection->selectedSongs()) {
			if(!songIdList.isEmpty())
				songIdList += ",";

			songIdList += QString::number(id);
		}

		/*db->exec("DELETE FROM song_tags WHERE song = ?", {currentSongId_});
		for(QString tag : ui->lnTags->toTags())
			db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {currentSongId_, tag});
	*/
		db->commitTransaction();
	});

	accept();
}
