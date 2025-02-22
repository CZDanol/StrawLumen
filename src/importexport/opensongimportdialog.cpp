#include "opensongimportdialog.h"
#include "ui_opensongimportdialog.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QDirIterator>
#include <QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QListView>
#include <QStandardPaths>
#include <QTreeView>
#include <QUuid>

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "gui/splashscreen.h"
#include "job/db.h"
#include "job/settings.h"
#include "presentation/native/presentation_song.h"
#include "rec/playlist.h"
#include "util/scopeexit.h"
#include "util/standarddialogs.h"

enum ConflictBehavior {
	cbSkip,
	cbOverwrite
};

OpenSongImportDialog *openSongImportDialog() {
	static OpenSongImportDialog *dlg = nullptr;
	if(!dlg)
		dlg = new OpenSongImportDialog(mainWindow);

	return dlg;
}

OpenSongImportDialog::OpenSongImportDialog(QWidget *parent) : QDialog(parent),
                                                              ui(new Ui::OpenSongImportDialog) {
	ui->setupUi(this);
}

OpenSongImportDialog::~OpenSongImportDialog() {
	delete ui;
}

void OpenSongImportDialog::show() {
	importFiles_.clear();
	importDirectory_.clear();
	ui->optSingleSongs->click();

	updateUi();
	QDialog::show();
}

void OpenSongImportDialog::show(const QStringList &files) {
	importFiles_ = files;
	updateUi();
	QDialog::show();
}

void OpenSongImportDialog::updateUi() {
	ui->btnSelectFiles->setText(importFiles_.isEmpty() ? tr("Vybrat položky...") : tr("%n souborů", nullptr, importFiles_.size()));
	ui->btnFolderStructure->setText(importDirectory_.isEmpty() ? tr("Vybrat složku...") : QFileInfo(importDirectory_).fileName());

	ui->btnImport->setEnabled(
	  (ui->optSingleSongs->isChecked() && !importFiles_.isEmpty()) || (ui->optDirectoryTree->isChecked() && !importDirectory_.isEmpty()));
}

QString OpenSongImportDialog::openSongToLumenSongFormat(const QString &openSongFormat) {
	QString result = openSongFormat;

	// Change sections formet
	{
		// Compatible sections format (that a is for the first slide of the section)
		static const QRegularExpression sectionRegex("\\[([VCBIOP][0-9]*)a?\\]\\s*");
		result.replace(sectionRegex, "{\\1}\n");

		// V1b, V1c, ... -> {---} (slide separator) and || -> {---}
		static const QRegularExpression sectionContinuationRegex("(\\s|\\n)*(\\[([VCBIO][0-9]*[b-z]?)\\]|\\|\\|)(\\s|\\n)*");
		result.replace(sectionContinuationRegex, "\n{---}\n");

		// [1] -> {V1}
		static const QRegularExpression simpleSectionRegex("\\[([1-9][0-9]*)\\]\\s*");
		result.replace(simpleSectionRegex, "{V\\1}\n");

		// Other -> {"XXX"}
		static const QRegularExpression otherSectionRegex("\\[([a-zA-Z0-9]+)\\]\\s*");
		result.replace(otherSectionRegex, "{\"\\1\"}\n");
	}

	// Change chords format
	static const QRegularExpression chordLineRegex("^(\\.[^\n]*\n)([^\n]*)$", QRegularExpression::MultilineOption);
	int offsetCorrection = 0;
	QRegularExpressionMatchIterator it = chordLineRegex.globalMatch(result);
	while(it.hasNext()) {
		const QRegularExpressionMatch m = it.next();

		result.remove(m.capturedStart(1) + offsetCorrection, m.capturedLength(1));
		offsetCorrection -= m.capturedLength(1);

		static const QRegularExpression chordRegex("\\S+");
		QRegularExpressionMatchIterator it2 = chordRegex.globalMatch(m.captured(1), 1);
		while(it2.hasNext()) {
			const QRegularExpressionMatch m2 = it2.next();
			const QString insertText = QString("[%1]").arg(m2.captured());
			result.insert(m.capturedStart(2) + qMin(m2.capturedStart(), m.capturedLength(2) - 1) + offsetCorrection, insertText);
			offsetCorrection += insertText.length();
		}
	}

	// Trim spaces on line beginnings and ends
	static const QRegularExpression trimmingRegex("^[ \t]+|[ \t]+$", QRegularExpression::MultilineOption);
	result.remove(trimmingRegex);
	result.remove('_');

	return result;
}

QString OpenSongImportDialog::importSong(const QString &filename, const int conflictBehavior, const QSet<QString> &tags, const bool addToPlaylist, QVector<QSharedPointer<Presentation>> &presentations) {
	QFile f(filename);
	if(!f.open(QIODevice::ReadOnly))
		return tr("Nepodařilo se otevřít soubor \"%1\".").arg(filename);

	const qlonglong lastEdit = QFileInfo(filename).lastModified().toSecsSinceEpoch();

	QString uid;
	{
		QCryptographicHash hash(QCryptographicHash::Sha3_224);
		hash.addData(f.readAll());
		uid = QString::fromUtf8(hash.result().toBase64());
		f.reset();
	}

	// Check existing
	QSqlRecord existingSong = db->selectRowDef("SELECT id, lastEdit FROM songs WHERE uid = ?", {uid});
	qlonglong songId = existingSong.isEmpty() ? -1 : existingSong.value("id").toLongLong();

	if(songId == -1 || conflictBehavior == cbOverwrite) {
		QDomDocument dom;
		if(!dom.setContent(&f))
			return tr("Nepodařilo se načíst soubor \"%1\".").arg(filename);

		f.close();

		QDomElement root = dom.documentElement();
		if(root.tagName() != "song")
			return tr("Soubor \"%1\" není formátu OpenSong.").arg(filename);

		const QString name = denullifyString(root.firstChildElement("title").text());

		// #: SONGS_TABLE_FIELDS
		// Denullify - the database has NOT NULL columns for error checking
		QHash<QString, QVariant> fields{
		  {"uid", uid},
		  {"lastEdit", lastEdit},

		  {"name", name},
		  {"standardized_name", standardizeSongName(name)},
		  {"author", denullifyString(root.firstChildElement("author").text())},
		  {"copyright", denullifyString(root.firstChildElement("copyright").text())},
		  {"slideOrder", denullifyString(root.firstChildElement("presentation").text())},
		  {"notes", ""}};

		fields.insert("content", openSongToLumenSongFormat(root.firstChildElement("lyrics").text().trimmed()));

		if(songId == -1)
			songId = db->insert("songs", fields).toLongLong();
		else
			db->update("songs", fields, "id = ?", {songId});

		DatabaseManager::updateSongFulltextIndex(db, songId);
	}

	for(const QString &tag: tags)
		db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, tag});

	if(addToPlaylist)
		presentations.append(Presentation_Song::createFromDb(songId));

	return nullptr;
}

void OpenSongImportDialog::on_btnClose_clicked() {
	reject();
}

void OpenSongImportDialog::on_btnImport_clicked() {
	const bool addToPlaylist = ui->cbAddToPlaylist->isChecked();
	const bool directoryTreeMode = ui->optDirectoryTree->isChecked();
	const QSet<QString> tags = ui->lnTags->toTags();

	QVector<QSharedPointer<Presentation>> presentations;
	const int conflictBehavior = ui->cmbConflictBehavior->currentIndex();

	splashscreen->asyncAction(tr("Import písní"), true, [&] {
		db->beginTransaction();
		SCOPE_EXIT(db->commitTransaction());

		if(directoryTreeMode) {
			QDir importDir(importDirectory_);
			QDirIterator dirsIt(importDirectory_, QDir::NoDotDot | QDir::Dirs);
			bool doBreak = false;

			while(dirsIt.hasNext() && !doBreak) {
				const QString dirPath = dirsIt.next();
				const QDir dir(dirPath);
				const QString dirName = dir.dirName();

				QSet<QString> extTags;
				extTags.unite(tags);
				extTags.insert(dirName);

				QDirIterator it(dirPath, QDir::Files);
				while(it.hasNext()) {
					QString filename = it.next();

					if(splashscreen->isStornoPressed())
						return;

					QString error = importSong(dir.absoluteFilePath(filename), conflictBehavior, extTags, addToPlaylist, presentations);
					if(!error.isEmpty()) {
						splashscreen->storno();
						standardErrorDialog(error);
						doBreak = true;
						break;
					}
				}
			}
		}
		else {
			for(int i = 0; i < importFiles_.size(); i++) {
				const QString filename = importFiles_[i];

				if(splashscreen->isStornoPressed())
					return;

				splashscreen->setProgress(i, importFiles_.size());

				QString error = importSong(filename, conflictBehavior, tags, addToPlaylist, presentations);
				if(!error.isEmpty()) {
					splashscreen->storno();
					standardErrorDialog(error);
					break;
				}
			}
		}
	});

	emit db->sigSongListChanged();

	if(!presentations.isEmpty() && (presentations.size() < 20 || standardConfirmDialog(tr("Importovaných písní je mnoho (%1). Opravdu je chcete všechny přidat do programu promítání?").arg(presentations.size())))) {
		mainWindow->presentationMode()->playlist()->addItems(presentations);
	}

	if(!splashscreen->isStornoPressed())
		standardSuccessDialog(tr("Písně byly importovány."));

	close();
}

void OpenSongImportDialog::on_btnSelectFiles_clicked() {
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

	ui->optSingleSongs->setChecked(true);
	updateUi();
}

void OpenSongImportDialog::on_btnFolderStructure_clicked() {
	static const QIcon icon(":/icons/16/Import_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::Directory);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	//dlg.setNameFilter(tr("Soubory OpenSong (*)"));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Import"));
	dlg.setDirectory(settings->value("openSongExportDirectory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("openSongExportDirectory", dlg.directory().absolutePath());
	importDirectory_ = dlg.selectedFiles().first();

	ui->optDirectoryTree->setChecked(true);
	updateUi();
}

void OpenSongImportDialog::on_optSingleSongs_clicked() {
	updateUi();
}

void OpenSongImportDialog::on_optDirectoryTree_clicked() {
	updateUi();
}
