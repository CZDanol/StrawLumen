#include "documentgenerationdialog.h"
#include "ui_documentgenerationdialog.h"

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
#include <QWebEngineSettings>

#include "gui/splashscreen.h"
#include "job/db.h"
#include "job/settings.h"
#include "rec/chord.h"
#include "rec/songsection.h"
#include "util/standarddialogs.h"
#include "util/scopeexit.h"
#include "util/execonmainthread.h"

#define DOCUMENT_GENERATION_SETTINGS_FACTORY(F) \
	F("generateToc", gbToc) F("tocColumns", sbTocColumns)\
	F("generateLyrics", gbLyrics) F("generateChords", cbGenerateChords) F("lyricsMode", cmbLyricsMode) F("lyricsColumns", sbLyricsColumns)\
	F("landscapeOrientation", cbLandscapeOrientation) F("pageBreakMode", cmbPageBreakMode) F("pageSize", cmbPageSize) F("pageMargins", sbPageMargins)\
	F("numberSongs", cbNumberSongs)\
	F("fontSize", cbFontSize)\
	F("openWhenDone", cbOpenWhenDone)

enum LyricsMode {
	lmContentDirected,
	lmSlideOrderDirected,
	lmSlideOrderDirected_SkipDuplicates
};

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

	DOCUMENT_GENERATION_SETTINGS_FACTORY(F)
#undef F
}

DocumentGenerationDialog::~DocumentGenerationDialog()
{
	settings->setValue("dialog.documentGeneration.geometry", saveGeometry());

	delete webPage_;
	delete webProfile_;

	delete ui;
}

void DocumentGenerationDialog::show()
{
	QDialog::show();
	ui->wgtSongSelection->focusSongList();
}

void DocumentGenerationDialog::setSelectedSongs(const QVector<qlonglong> &songIds)
{
	ui->wgtSongSelection->setSelectedSongsIfReasonable(songIds);
}

void DocumentGenerationDialog::generate(const QVector<qlonglong> &songIds)
{
	splashscreen->show(tr("Generování zpěvníku"), false);

	if(!webPage_) {
		qApp->processEvents();
		qApp->processEvents();// Ensure the splashScreen is drawn - creating QWebEnginePage freezes the application for a while :(

		webProfile_ = new QWebEngineProfile(this);

		QWebEngineSettings *webSettings = webProfile_->settings();
		webSettings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
		webSettings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
		webSettings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
		webSettings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);
		webSettings->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, false);
		webSettings->setAttribute(QWebEngineSettings::WebGLEnabled, false);

		webPage_ = new QWebEnginePage(webProfile_, this);
		connect(webPage_, SIGNAL(loadFinished(bool)), this, SLOT(onPageLoaded(bool)));
	}

	{
		QJsonObject json;

		json["generateTableOfContents"] = ui->gbToc->isChecked();
		json["generateLyrics"] = ui->gbLyrics->isChecked();
		json["generateChords"] = ui->cbGenerateChords->isChecked();
		json["generateNumbering"] = ui->cbNumberSongs->isChecked();

		json["tocColumnCount"] = ui->sbTocColumns->value();
		json["lyricsColumnCount"] = ui->sbLyricsColumns->value();
		json["pageBreakMode"] = ui->cmbPageBreakMode->currentIndex();

		json["fontSize"] = ui->cbFontSize->value();

		json["tocTitle"] = tr("Obsah");

		{
			QJsonArray jsonSongs;

			for(qlonglong songId : songIds)
				generateSong(songId, jsonSongs);

			json["songs"] = jsonSongs;
		}

		jsCode_ = QString("generateDocument(%1);").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact)));
	}

	webPage_->load(QUrl::fromLocalFile(QDir(qApp->applicationDirPath()).absoluteFilePath("../etc/songbookTemplate.html")));
}

void DocumentGenerationDialog::generateSong(qlonglong songId, QJsonArray &output)
{
	QSqlRecord r = db->selectRow("SELECT name, author, content, notes, slideOrder FROM songs WHERE id = ?", {songId});

	QJsonObject jsonSong;

	jsonSong["name"] = r.value("name").toString();
	jsonSong["author"] = r.value("author").toString();
	jsonSong["notes"] = r.value("notes").toString();

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
			sections.append(sectionMap.value(SongSection(slide, false).standardName(), SongSectionWithContent()));
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

				jsonChord["chordName"] = chs.chord.toString(chs.chord.isFlat());

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
	if(!result) {
		splashscreen->close();
		return standardErrorDialog(tr("Neznámá chyba při vytváření zpěvníku (nepodařilo se načíst šablonu)."));
	}

	webPage_->runJavaScript(jsCode_);

	const int m = ui->sbPageMargins->value();

	QPageLayout pageLayout;
	pageLayout.setUnits(QPageLayout::Millimeter);
	pageLayout.setPageSize(QPageSize(ui->cmbPageSize->currentData().value<QPageSize::PageSizeId>()));
	pageLayout.setOrientation(ui->cbLandscapeOrientation->isChecked() ? QPageLayout::Landscape : QPageLayout::Portrait);
	pageLayout.setMargins(QMarginsF(m,m,m,m));

	QPointer<DocumentGenerationDialog> thisPtr(this);

	if(outputFilePath_.endsWith(".html")) {
		webPage_->toHtml([this, thisPtr] (const QString &str) {
			splashscreen->close();

			if(thisPtr.isNull())
				return;

			QFile f(outputFilePath_);
			if(!f.open(QIODevice::WriteOnly))
				return standardErrorDialog(tr("Nepodařilo se otevřít soubor '%1' pro zápis.").arg(outputFilePath_));

			f.write(str.toUtf8());

			if(ui->cbOpenWhenDone->isChecked())
				execOnMainThread([=]{QDesktopServices::openUrl(QUrl::fromLocalFile(outputFilePath_));});
			else
				standardSuccessDialog(tr("Zpěvník byl uložen do \"%1\"").arg(outputFilePath_));
		});
	}
	else {
		webPage_->printToPdf([this, thisPtr](const QByteArray &data){
			if(thisPtr.isNull())
				return;

			onPdfGenerated(data);
		}, pageLayout);
	}
}

void DocumentGenerationDialog::onPdfGenerated(const QByteArray &data)
{
	splashscreen->close();

	splashscreen->asyncAction(tr("Vytváření PDF souboru"), false, [&]{
		if(data.isEmpty())
			return standardErrorDialog(tr("Neznámá chyba při vytváření zpěvníku (nepodařilo se připravit PDF data)."));

		QFile f(outputFilePath_);
		if(!f.open(QIODevice::WriteOnly))
			return standardErrorDialog(tr("Nepodařilo se otevřít soubor \"%1\" pro zápis.").arg(outputFilePath_));

		if(f.write(data) != data.length())
			return standardErrorDialog(tr("Neznámá chyba při vytváření zpěvníku (nebyla zapsána všechna data).").arg(outputFilePath_));

		if(ui->cbOpenWhenDone->isChecked())
			execOnMainThread([=]{QDesktopServices::openUrl(QUrl::fromLocalFile(outputFilePath_));});
		else
			standardSuccessDialog(tr("Zpěvník byl uložen do \"%1\"").arg(outputFilePath_));
	});
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
		dlg.setNameFilters({tr("Soubory PDF (*.pdf)"), tr("Soubory HTML (*.html)")});
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

DocumentGenerationDialog *documentGenerationDialog()
{
	static DocumentGenerationDialog *dlg = nullptr;
	if(!dlg)
		dlg = new DocumentGenerationDialog(mainWindow);

	return dlg;
}
