#include "documentgenerationdialog.h"
#include "ui_documentgenerationdialog.h"

#include "job/db.h"
#include "rec/chord.h"
#include "rec/songsection.h"

#include <QLayout>
#include <QCoreApplication>
#include <QDir>
#include <QWebEnginePage>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

DocumentGenerationDialog *documentGenerationDialog = nullptr;

DocumentGenerationDialog::DocumentGenerationDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DocumentGenerationDialog)
{
	ui->setupUi(this);
	/*connect(&view_, &QWebEngineView::loadFinished, this, &DocumentGenerationDialog::onLoaded);
	layout()->addWidget(&view_);*/
}

DocumentGenerationDialog::~DocumentGenerationDialog()
{
	delete ui;
}

void DocumentGenerationDialog::onLoaded()
{
	/*view_.page()->runJavaScript(jqueryJs);
	view_.page()->runJavaScript(genJs);*/
}

void DocumentGenerationDialog::on_pushButton_clicked()
{
	/*view_.load(QUrl::fromLocalFile(QDir(qApp->applicationDirPath()).absoluteFilePath("../document_templates/test.html")));

	QFile jqueryFile(QDir(qApp->applicationDirPath()).absoluteFilePath("../document_templates/jquery.js"));
	jqueryFile.open(QIODevice::ReadOnly);

	jqueryJs = QString::fromUtf8(jqueryFile.readAll());
	//QWebEngineScript jqueryScript;
	//jqueryScript.setSourceCode( QString::fromUtf8(jqueryFile.readAll()));
	//jqueryScript.setWorldId(QWebEngineScript::UserWorld);
	//view_.page()->scripts().insert(jqueryScript);

	QJsonObject json;
	json["generateTableOfContents"] = true;
	json["generateSongs"] = true;
	json["generateChords"] = true;
	json["generateNumbering"] = true;
	json["tocColumnCount"] = ui->sbTocColumnCount->value();
	json["contentColumnCount"] = ui->sbContentColumnCount->value();
	json["pageBreakMode"] = ui->cmbPageBreakMode->currentIndex();

	json["tocTitle"] = tr("Obsah");

	{
		QJsonArray jsonSongs;

		QSqlQuery q = db->selectQuery("SELECT * FROM songs ORDER BY name ASC");
		while(q.next()) {
			QJsonObject jsonSong;

			jsonSong["name"] = q.value("name").toString();
			jsonSong["author"] = q.value("author").toString();

			static const QRegularExpression compactSpacesRegex("[ \t]+");
			QString songContent = q.value("content").toString().trimmed();
			songContent.replace(compactSpacesRegex, " ");

			jsonSong["rawContent"] = songContent;

			{
				QJsonArray jsonSections;

				for(const SongSectionWithContent &ss : songSectionsWithContent(songContent)) {
					QJsonObject jsonSection;

					QString sectionContent = ss.content.trimmed();
					sectionContent.replace(compactSpacesRegex, " ");

					jsonSection["standardName"] = ss.section.standardName();
					jsonSection["userFriendlyName"] = ss.section.userFriendlyName();

					// Split by chords
					{
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

					jsonSections.append(jsonSection);
				}

				jsonSong["sections"] = jsonSections;
			}


			jsonSongs.append(jsonSong);
		}

		json["songs"] = jsonSongs;
	}

	//QWebEngineScript genScript;
	//genScript.setSourceCode(QString("$(document).ready(function(){generateDocument(%1);});").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact))));
	genJs = QString("generateDocument(%1);").arg(QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Compact)));*/
}

void DocumentGenerationDialog::on_pushButton_2_clicked()
{
	//view_.page()->printToPdf(QString("test.pdf"));
}
