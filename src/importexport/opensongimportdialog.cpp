#include "opensongimportdialog.h"
#include "ui_opensongimportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QDomDocument>
#include <QUuid>
#include <QFile>
#include <QDateTime>

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "gui/splashscreen.h"
#include "job/db.h"
#include "job/settings.h"
#include "rec/playlist.h"
#include "presentation/presentation.h"
#include "presentation/native/presentation_song.h"
#include "util/scopeexit.h"
#include "util/standarddialogs.h"

OpenSongImportDialog *openSongImportDialog()
{
	static OpenSongImportDialog *dlg = nullptr;
	if(!dlg)
		dlg = new OpenSongImportDialog(mainWindow);

	return dlg;
}


OpenSongImportDialog::OpenSongImportDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OpenSongImportDialog)
{
	ui->setupUi(this);
}

OpenSongImportDialog::~OpenSongImportDialog()
{
	delete ui;
}

void OpenSongImportDialog::show()
{
	importFiles_.clear();
	updateUi();
	QDialog::show();
}

void OpenSongImportDialog::updateUi()
{
	ui->btnSelectFiles->setText(" " + (importFiles_.isEmpty() ? tr("Vybrat soubory...") : tr("%n souborů", nullptr, importFiles_.size())));
	ui->btnImport->setEnabled(!importFiles_.isEmpty());
}

void OpenSongImportDialog::on_btnClose_clicked()
{
	reject();
}

void OpenSongImportDialog::on_btnImport_clicked()
{
	const bool addToPlaylist = ui->cbAddToPlaylist->isChecked();
	const QSet<QString> tags = ui->lnTags->toTags();

	QVector<QSharedPointer<Presentation> > presentations;

	splashscreen->asyncAction(tr("Import písní"), true, [&]{
		db->beginTransaction();
		SCOPE_EXIT(db->commitTransaction());

		for(int i = 0; i < importFiles_.size(); i++) {
			const QString filename = importFiles_[i];

			if(splashscreen->isStornoPressed())
				return;

			splashscreen->setProgress(i, importFiles_.size());

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
			// Denullify - the database has NOT NULL columns for error checking
			QHash<QString,QVariant> fields {
				{"uid", QUuid::createUuid().toString()},
				{"lastEdit", QDateTime::currentSecsSinceEpoch()},

				{"name", denullifyString(root.firstChildElement("title").text())},
				{"author", denullifyString(root.firstChildElement("author").text())},
				{"copyright", denullifyString(root.firstChildElement("copyright").text())},
				{"slideOrder", denullifyString(root.firstChildElement("presentation").text())}
			};

			QString content = root.firstChildElement("lyrics").text().trimmed();

			// Change sections formet
			static const QRegularExpression sectionRegex("\\[([VCPBTIOSNR][0-9]*)\\]\\s*");
			content.replace(sectionRegex, "{\\1}\n");

			static const QRegularExpression simpleSectionRegex("\\[([0-9]*)\\]\\s*");
			content.replace(simpleSectionRegex, "{V\\1}\n");

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
					content.insert(m.capturedStart(2)+qMin(m2.capturedStart(),m.capturedLength(2)-1)+offsetCorrection, insertText);
					offsetCorrection += insertText.length();
				}
			}

			// Trim spaces on line beginnings and ends
			static const QRegularExpression trimmingRegex("^[ \t]+|[ \t]+$", QRegularExpression::MultilineOption);
			content.remove(trimmingRegex);
			content.remove('_');

			fields.insert("content", content);

			const qlonglong songId = db->insert("songs", fields).toLongLong();
			db->updateSongFulltextIndex(songId);

			for(const QString &tag : tags)
				db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, tag});

			if(addToPlaylist)
				presentations.append(Presentation_Song::createFromDb(songId));
		}
	});

	emit db->sigSongListChanged();

	if(!presentations.isEmpty() && (
			 presentations.size() < 20
			 || standardConfirmDialog(tr("Importovaných písní je mnoho (%1). Opravdu je chcete všechny přidat do programu promítání?").arg(presentations.size()))
			 )) {
			mainWindow->presentationMode()->playlist()->addItems(presentations);
	}

	if(!splashscreen->isStornoPressed())
		standardSuccessDialog(tr("Písně byly importovány."));

	close();
}

void OpenSongImportDialog::on_btnSelectFiles_clicked()
{
	static const QIcon icon(":/icons/16/Import_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Soubory OpenSong (*)"));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import"));
	dlg.setDirectory(settings->value("openSongExportDirectory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("openSongExportDirectory", dlg.directory().absolutePath());
	importFiles_ = dlg.selectedFiles();

	updateUi();
}