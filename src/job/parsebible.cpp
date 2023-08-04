#include "parsebible.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QCoreApplication>
#include <QDir>
#include <QRegularExpression>

#include "rec/biblebook.h"
#include "util/standarddialogs.h"
#include "job/db.h"
#include "gui/splashscreen.h"

static QStringList bibleImportList;

bool parseBible(const QString &filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly)) {
        standardErrorDialog(QObject::tr("Nepodařilo se otevřít soubor překladu Bible '%1'").arg(filename));
        return false;
    }

    QXmlStreamReader xml(&f);

    if(!xml.readNextStartElement() || xml.name() != QStringLiteral("XMLBIBLE")) {
        standardErrorDialog(QObject::tr("Soubor '%1' není validní soubor překladu Bible (E00)").arg(filename));
        return false;
    }

    QString translationName, translationId;
    QString defaultName = xml.attributes().value("biblename").toString();

    db->beginTransaction();
    bool isError = false;

    while(xml.readNextStartElement()) {
        // Parse the header
        if(xml.name() == QStringLiteral("INFORMATION")) {
            while(xml.readNextStartElement()) {
                if(xml.name() == QStringLiteral("title"))
                    translationName = xml.readElementText();

                else if(xml.name() == QStringLiteral("identifier")) {
                    if(!translationId.isEmpty()) {
                        standardErrorDialog(QObject::tr("Neplatný formát souboru Bible '%1' (TRNEM)").arg(filename));
                        isError = true;
                        break;
                    }

                    translationId = xml.readElementText();

                    // Remove all characters not supported by the bibleverse regex
                    static const QRegularExpression rx("(?!\\p{L}|[0-9_]).", QRegularExpression::UseUnicodePropertiesOption);
                    translationId.remove(rx);
                    if(translationId.isEmpty()) {
                        standardErrorDialog(QObject::tr("Neplatný formát souboru Bible '%1' (TREM)").arg(filename));
                        isError = true;
                        break;
                    }

                    deleteBible(translationId);

                } else
                    xml.skipCurrentElement();
            }
        }

        // Parse the contents
        else if(xml.name() == QStringLiteral("BIBLEBOOK") || xml.name() == QStringLiteral("biblebook")) {
            // There might not be the "INFORMATION" header
            if(translationId.isEmpty()) {
                translationId = defaultName;
                translationName = defaultName;

                deleteBible(translationId);

                if(translationId.isEmpty()) {
                    standardErrorDialog(QObject::tr("Neplatný formát souboru Bible '%1' (TREMND)").arg(filename));
                    isError = true;
                    break;
                }
            }

            const auto bookAttrs = xml.attributes();
            const int bookNumId = bookAttrs.value("bnumber").toInt();
            const QString bookName = bookAttrs.hasAttribute("bname") ? bookAttrs.value("bname").toString() : getBibleBook(bookNumId).name;
            int maxChapter = 0;

            while(xml.readNextStartElement()) {
                if(xml.name() == QStringLiteral("CHAPTER") || xml.name() == QStringLiteral("chapter")) {
                    const int chapterId = xml.attributes().value("cnumber").toInt();

                    if(chapterId > maxChapter)
                        maxChapter = chapterId;

                    while(xml.readNextStartElement()) {
                        if(xml.name() == QStringLiteral("VERS") || xml.name() == QStringLiteral("vers")) {
                            const int verseId = xml.attributes().value("vnumber").toInt();

                            static const QRegularExpression toSpaceRegex("[\t\n]"), collateSpacesRegex("  +");
                            static const QRegularExpression hebrewWeirdSymbolRegex("׃$"); // Weird hebrew buggy symbol appearing in the 2014 OSMHB bible
                            const QString verseText = xml.readElementText(QXmlStreamReader::IncludeChildElements).replace(toSpaceRegex, " ").replace(collateSpacesRegex, " ").remove(hebrewWeirdSymbolRegex).trimmed();
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

    if(!translationId.isEmpty())
        db->insert("INSERT INTO bible_translations(translation_id, name) VALUES(?, ?)", {translationId, translationName});

    db->commitTransaction();
    return !isError;
}

void checkBibleImport()
{
    const QDir dir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../bible");

    if(db->selectValue("SELECT COUNT(*) FROM bible_translations").toInt() == 0)
        bibleImportList = dir.entryList({"*.xml"}, QDir::Files);

    if(bibleImportList.empty())
        return;

    splashscreen->asyncAction(QObject::tr("Import překladů Bible"), false, [&]{
        for(int i = 0; i < bibleImportList.size(); i++) {
            splashscreen->setProgress(i,bibleImportList.size());
            parseBible(dir.absoluteFilePath(bibleImportList[i]));
        }
    });

    emit db->sigBibleTranslationsChanged();
}

void deleteBible(const QString &translationId)
{
    db->exec("DELETE FROM bible_verses_fulltext WHERE docid IN (SELECT rowid FROM bible_translation_verses WHERE translation_id = ?)", {translationId});
    db->exec("DELETE FROM bible_translation_verses WHERE translation_id = ?", {translationId});
    db->exec("DELETE FROM bible_translation_books WHERE translation_id = ?", {translationId});
    db->exec("DELETE FROM bible_translations WHERE translation_id = ?", {translationId});
}

void forceImportBibles(const QStringList &filenames)
{
    bibleImportList += filenames;
}
