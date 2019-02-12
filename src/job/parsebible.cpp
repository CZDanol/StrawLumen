#include "parsebible.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QDir>

#include "rec/biblebook.h"
#include "util/standarddialogs.h"
#include "job/db.h"
#include "gui/splashscreen.h"

void parseBible(const QString &filename)
{
	QFile f(filename);
	if(!f.open(QIODevice::ReadOnly))
		return standardErrorDialog(QObject::tr("Nepodařilo se otevřít soubor překladu bible '%1'").arg(filename));

	QXmlStreamReader xml(&f);

	if(!xml.readNextStartElement() || xml.name() != "XMLBIBLE")
		return standardErrorDialog(QObject::tr("Soubor '%1' není validní soubor překladu Bible (E00)").arg(filename));

	QString translationName, translationId;

	db->beginTransaction();

	while(xml.readNextStartElement()) {
		// Parse the header
		if(xml.name() == "INFORMATION") {
			while(xml.readNextStartElement()) {
				if(xml.name() == "title")
					translationName = xml.readElementText();
				else if(xml.name() == "identifier")
					translationId = xml.readElementText();
				else
					xml.skipCurrentElement();
			}
		}
		// Parse the contents
		else if(xml.name() == "BIBLEBOOK" || xml.name() == "biblebook") {
			const auto bookAttrs = xml.attributes();
			const int bookNumId = bookAttrs.value("bnumber").toInt();
			const QString bookName = bookAttrs.hasAttribute("bname") ? bookAttrs.value("bname").toString() : getBibleBook(bookNumId).name;
			int maxChapter = 0;

			while(xml.readNextStartElement()) {
				if(xml.name() == "CHAPTER" || xml.name() == "chapter") {
					const int chapterId = xml.attributes().value("cnumber").toInt();

					if(chapterId > maxChapter)
						maxChapter = chapterId;

					while(xml.readNextStartElement()) {
						if(xml.name() == "VERS" || xml.name() == "vers") {
							const int verseId = xml.attributes().value("vnumber").toInt();
							const QString verseText = xml.readElementText(QXmlStreamReader::IncludeChildElements).replace('\t', ' ');
							const QString collatedVerseText = collate(verseText);

							const qlonglong rowId = db->insert("INSERT INTO bible_translation_verses(translation_id, book_id, chapter, verse, text) VALUES(?, ?, ?, ?, ?)", {translationId, bookNumId, chapterId, verseId, verseText}).toLongLong();
							db->insert("INSERT INTO bible_verses_fulltext(docid, text) VALUES(?, ?)", {rowId, collatedVerseText});

						} else
							xml.skipCurrentElement();
					}

				} else
					xml.skipCurrentElement();
			}

			db->insert("INSERT INTO bible_translation_books(translation_id, book_id, name, max_chapter) VALUES(?, ?, ?, ?)", {translationId, bookNumId, bookName, maxChapter});
		}
		else
			xml.skipCurrentElement();
	}

	db->insert("INSERT INTO bible_translations(translation_id, name) VALUES(?, ?)", {translationId, translationName});

	db->commitTransaction();
	emit db->sigBibleTranslationsChanged();
}

void checkBibleImport()
{
	if(db->selectValue("SELECT COUNT(*) FROM bible_translations").toInt() > 0)
		return;

	const QDir dir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../bible");
	QStringList importableBibles = dir.entryList({"*.xml"}, QDir::Files);
	if(importableBibles.empty())
		return;

	splashscreen->asyncAction(QObject::tr("Import překladů Bible"), false, [&]{
		for(int i = 0; i < importableBibles.size(); i++) {
			splashscreen->setProgress(i,importableBibles.size());
			parseBible(dir.absoluteFilePath(importableBibles[i]));
		}
	});
}
