#include "bulkeditsongsdialog.h"
#include "ui_bulkeditsongsdialog.h"

#include "gui/mainwindow.h"
#include "gui/splashscreen.h"
#include "job/db.h"
#include "util/standarddialogs.h"

BulkEditSongsDialog::BulkEditSongsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BulkEditSongsDialog) {
	ui->setupUi(this);
}

BulkEditSongsDialog::~BulkEditSongsDialog() {
	delete ui;
}

void BulkEditSongsDialog::setSelectedSongs(const QVector<qlonglong> &songIds) {
	ui->wgtSongSelection->setSelectedSongs(songIds);
}

BulkEditSongsDialog *bulkEditSongsDialog() {
	static BulkEditSongsDialog *result = new BulkEditSongsDialog(mainWindow);
	return result;
}

void BulkEditSongsDialog::on_btnStorno_clicked() {
	reject();
}

void BulkEditSongsDialog::on_btnGenerate_clicked() {
	const auto sourceSongIDs = ui->wgtSongSelection->selectedSongs();
	bool anySongLocked = false;

	splashscreen->asyncAction(tr("Provádění úprav"), false, [&]() {
		db->beginTransaction();

		QSqlQuery q(db->database());

		QVector<qlonglong> songIDs;
		songIDs.reserve(sourceSongIDs.size());

		// Filter out locked songs
		{
			q.prepare("SELECT locked FROM songs WHERE id = ?");
			for(const auto songID: sourceSongIDs) {
				q.bindValue(0, songID);
				q.exec();

				if(!q.value(0).toBool()) {
					songIDs += songID;
				}
				else {
					anySongLocked = true;
				}
			}
		}

		// Remove tags
		{
			const auto tags = ui->lnRemoveTags->toTags();
			q.prepare("DELETE FROM song_tags WHERE song = ? AND tag = ?");

			for(const qlonglong song: songIDs) {
				q.bindValue(0, song);

				for(const QString &tag: tags) {
					q.bindValue(1, tag);
					q.exec();
				}
			}
		}

		// Add tags
		{
			const auto tags = ui->lnAddTags->toTags();
			q.prepare("INSERT INTO song_tags(song, tag) VALUES(?, ?)");

			for(const qlonglong song: songIDs) {
				q.bindValue(0, song);

				for(const QString &tag: tags) {
					q.bindValue(1, tag);
					q.exec();
				}
			}
		}

		db->commitTransaction();
	});

	for(const qlonglong song: ui->wgtSongSelection->selectedSongs())
		emit db->sigSongChanged(song);

	emit db->sigSongListChanged();

	if(anySongLocked) {
		standardSuccessDialog(tr("Některé vybrané písně byly zamčené: úpravy byly provedeny pouze pro nezamčené písně."), this);
	}
	else {
		standardSuccessDialog(tr("Úpravy byly provedeny."), this);
	}
}
