#include "documentgenerationdialog.h"
#include "ui_documentgenerationdialog.h"

#include "gui/splashscreen.h"
#include "job/db.h"
#include "job/settings.h"
#include "rec/chord.h"
#include "rec/songsection.h"
#include "util/standarddialogs.h"
#include "util/scopeexit.h"

#include <QLayout>
#include <QCoreApplication>
#include <QDir>
#include <QWebEnginePage>
#include <QFileDialog>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPageLayout>
#include <QPageSize>
#include <QPointer>
#include <QDesktopServices>

#define SETTINGS_FACTORY(F) \
	F("generateToc", gbToc) F("tocColumns", sbTocColumns)\
	F("generateLyrics", gbLyrics) F("generateChords", cbGenerateChords) F("lyricsMode", cmbLyricsMode) F("lyricsColumns", sbLyricsColumns)\
	F("landscapeOrientation", cbLandscapeOrientation) F("pageBreakMode", cmbPageBreakMode) F("pageSize", cmbPageSize) F("pageMargins", sbPageMargins)\
	F("numberSongs", cbNumberSongs) F("numberPages", cbNumberPages)\
	F("openWhenDone", cbOpenWhenDone)

enum LyricsMode {
	lmContentDirected,
	lmSlideOrderDirected,
	lmSlideOrderDirected_SkipDuplicates
};

DocumentGenerationDialog *documentGenerationDialog = nullptr;

DocumentGenerationDialog::DocumentGenerationDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DocumentGenerationDialog)
{
	ui->setupUi(this);

	restoreGeometry(settings->value("dialog.documentGeneration.geometry").toByteArray());

	ui->cmbPageSize->addItem("A2", QPageSize::A2);
	ui->cmbPageSize->addItem("A3", QPageSize::A3);
	ui->cmbPageSize->addItem("A4", QPageSize::A4);
	ui->cmbPageSize->addItem("A5", QPageSize::A5);
	ui->cmbPageSize->addItem("A6", QPageSize::A6);

	ui->cmbPageSize->setCurrentText(tr("A4", "Default page size"));

#define F(settingsName, uiControl)\
		loadSetting("dialog.documentGeneration." settingsName, ui->uiControl);\
		connect(ui->uiControl, settingsControlChangeSignal(ui->uiControl), [this]{\
			saveSetting("dialog.documentGeneration." settingsName, ui->uiControl);\
		});

	SETTINGS_FACTORY(F)
#undef F
}

DocumentGenerationDialog::~DocumentGenerationDialog()
{
	settings->setValue("dialog.documentGeneration.geometry", saveGeometry());

	delete ui;
}

void DocumentGenerationDialog::showEvent(QShowEvent *e)
{
	ui->wgtSongSelection->clearSelection();
	QDialog::showEvent(e);
}

void DocumentGenerationDialog::generate(const QVector<qlonglong> &songIds)
{
	if(!page_) {
		page_ = new QWebEnginePage(this);
		connect(page_, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
	}

	splashscreen->asyncAction(tr("Příprava dat"), false, [&](){
		QJsonObject json;

		json["generateTableOfContents"] = ui->gbToc->isChecked();
		json["generateLyrics"] = ui->gbLyrics->isChecked();
		json["generateChords"] = ui->cbGenerateChords->isChecked();
		json["generateNumbering"] = ui->cbNumberSongs->isChecked();

		json["tocColumnCount"] = ui->sbTocColumns->value();
		json["lyricsColumnCount"] = ui->sbLyricsColumns->value();
		json["pageBreakMode"] = ui->cmbPageBreakMode->currentIndex();

		json["tocTitle"] = tr("Obsah");

		{
			QJsonArray jsonSongs;

			for(qlonglong songId : songIds)
				generateSong(songId, jsonSongs);

			json["songs"] = jsonSongs;
		}

		jsCode_ = QString("generateDocument(%1);").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact)));
	});

	splashscreen->show(tr("Generování zpěvníku"), false);
	page_->load(QUrl::fromLocalFile(QDir(qApp->applicationDirPath()).absoluteFilePath("../etc/songbookTemplate.html")));
}

void DocumentGenerationDialog::generateSong(qlonglong songId, QJsonArray &output)
{
	QSqlRecord r = db->selectRow("SELECT name, author, content, slideOrder FROM songs WHERE id = ?", {songId});

	QJsonObject jsonSong;

	jsonSong["name"] = r.value("name").toString();
	jsonSong["author"] = r.value("author").toString();

	static const QRegularExpression compactSpacesRegex("[ \t]+");
	QString songContent = r.value("content").toString().trimmed();
	songContent.replace(compactSpacesRegex, " ");
	songContent.remove(songSlideSeparatorRegex());

	jsonSong["rawContent"] = songContent;

	QJsonArray jsonSections;

	QVector<SongSectionWithContent> originalSections = songSectionsWithContent(songContent);
	QVector<SongSectionWithContent> sections;
	const QString slideOrderStr = r.value("slideOrder").toString().trimmed();
	const int lyricsMode = ui->cmbLyricsMode->currentIndex();

	if(lyricsMode == lmContentDirected || slideOrderStr.isEmpty())
		sections = originalSections;

	else {
		const QStringList slideOrder = slideOrderStr.split(' ');

		QHash<QString,SongSectionWithContent> sectionMap;
		for(const SongSectionWithContent &ss : originalSections)
			sectionMap.insert(ss.section.standardName(), ss);

		for(const QString &slide : slideOrder)
			sections.append(sectionMap.value(slide, SongSectionWithContent()));
	}

	QSet<QString> usedSections;

	for(const SongSectionWithContent &ss : sections) {
		QJsonObject jsonSection;

		QString sectionContent = ss.content.trimmed();
		sectionContent.replace(compactSpacesRegex, " ");

		jsonSection["standardName"] = ss.section.standardName();
		jsonSection["userFriendlyName"] = ss.section.userFriendlyName();
		jsonSection["shorthandName"] = ss.section.shorthandName();

		if(lyricsMode == lmSlideOrderDirected_SkipDuplicates && usedSections.contains(ss.section.standardName())) {
			jsonSection["content"] = QJsonArray();

		// Split by chords
		} else {
			QString contentWithoutChords = sectionContent;
			QJsonArray jsonChords;
			int offsetCorrection = 0;
			int chordSectionStart = 0;

			QJsonObject jsonChord;
			jsonChord["chordName"] = QJsonValue();

			const auto endFunc = [&](int pos) {
				if(pos == chordSectionStart && jsonChord["chordName"].toString().isEmpty())
					return;

				jsonChord["textFollowing"] = sectionContent.mid(chordSectionStart, pos-chordSectionStart);

				jsonChords.append(jsonChord);
			};

			for(const ChordInSong &chs : songChords(sectionContent)) {
				endFunc(chs.annotationPos);
				chordSectionStart = chs.annotationPos + chs.annotationLength;

				jsonChord["chordName"] = chs.chord.toString();

				contentWithoutChords.remove(chs.annotationPos+offsetCorrection, chs.annotationLength);
				offsetCorrection -= chs.annotationLength;
			}

			endFunc(sectionContent.length());

			jsonSection["content"] = jsonChords;
		}

		usedSections.insert(ss.section.standardName());

		jsonSections.append(jsonSection);
	}

	jsonSong["sections"] = jsonSections;

	output.append(jsonSong);
}

void DocumentGenerationDialog::onPageLoaded(bool result)
{
	splashscreen->close();

	if(!result)
		return standardErrorDialog(tr("Neznámá chyba při vytváření zpěvníku (nepodařilo se načíst šablonu)."));

	splashscreen->show(tr("Export do PDF"), false);

	page_->runJavaScript(jsCode_);

	const int m = ui->sbPageMargins->value();

	QPageLayout pageLayout;
	pageLayout.setUnits(QPageLayout::Millimeter);
	pageLayout.setPageSize(QPageSize(ui->cmbPageSize->currentData().value<QPageSize::PageSizeId>()));
	pageLayout.setOrientation(ui->cbLandscapeOrientation->isChecked() ? QPageLayout::Landscape : QPageLayout::Portrait);
	pageLayout.setMargins(QMarginsF(m,m,m,m));

	QPointer<DocumentGenerationDialog> thisPtr(this);
	page_->printToPdf([this, thisPtr](const QByteArray &data){
		if(thisPtr.isNull())
			return;

		onPdfGenerated(data);
	}, pageLayout);
}

void DocumentGenerationDialog::onPdfGenerated(const QByteArray &data)
{
	splashscreen->close();

	if(data.isNull())
		return standardErrorDialog(tr("Neznámá chyba při vytváření zpěvníku (nepodařilo se připravit PDF data)."));

	QFile f(outputFilePath_);
	if(!f.open(QIODevice::WriteOnly))
		return standardErrorDialog(tr("Nepodařilo se otevřít soubor \"%1\" pro zápis.").arg(f.fileName()));

	if(f.write(data) != data.length())
		return standardErrorDialog(tr("Neznámá chyba při vytváření zpěvníku (nebyla zapsána všechna data).").arg(f.fileName()));

	f.close();

	if(ui->cbOpenWhenDone)
		QDesktopServices::openUrl(QUrl::fromLocalFile(outputFilePath_));
}


void DocumentGenerationDialog::on_btnStorno_clicked()
{
	reject();
}

void DocumentGenerationDialog::on_btnGenerate_clicked()
{
	if(!ui->gbToc->isChecked() && !ui->gbLyrics->isChecked())
		return standardErrorDialog(tr("Aktuální konfigurace by vytvořila prázdný dokument, protože není zvoleno ani generování obsahu, ani generování slov písní."));

	QVector<qlonglong> songIds = ui->wgtSongSelection->selectedSongs();
	if(songIds.isEmpty())
		return standardErrorDialog(tr("Není zvolena ani jedna píseň."));

	{
		static const QIcon icon(":/icons/16/Moleskine_16px.png");

		QFileDialog dlg(this);
		dlg.setFileMode(QFileDialog::AnyFile);
		dlg.setAcceptMode(QFileDialog::AcceptSave);
		dlg.setNameFilter(tr("Soubory PDF (*.pdf)"));
		dlg.setWindowIcon(icon);
		dlg.setWindowTitle(tr("Vytvoření zpěvníku"));
		dlg.setDirectory(settings->value("dialog.documentGeneration.directory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

		if(!dlg.exec())
			return;

		settings->setValue("dialog.documentGeneration.directory", dlg.directory().absolutePath());
		outputFilePath_ = dlg.selectedFiles().first();
	}

	generate(songIds);
}
