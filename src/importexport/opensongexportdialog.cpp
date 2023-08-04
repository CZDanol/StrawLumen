#include "opensongexportdialog.h"
#include "ui_opensongexportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QDomDocument>
#include <QFile>
#include <QDebug>

#include "gui/splashscreen.h"
#include "job/db.h"
#include "job/settings.h"
#include "rec/chord.h"
#include "rec/songsection.h"
#include "util/standarddialogs.h"
#include "gui/mainwindow.h"

OpenSongExportDialog *openSongExportDialog()
{
    static OpenSongExportDialog *dlg = nullptr;
    if(!dlg)
        dlg = new OpenSongExportDialog(mainWindow);

    return dlg;
}

OpenSongExportDialog::OpenSongExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenSongExportDialog)
{
    ui->setupUi(this);
}

OpenSongExportDialog::~OpenSongExportDialog()
{
    delete ui;
}

void OpenSongExportDialog::setSelectedSongs(const QVector<qlonglong> &songIds)
{
    ui->wgtSongSelection->setSelectedSongs(songIds);
}

bool OpenSongExportDialog::exportSong(const qlonglong songId, const QDir &outputDir)
{
    const QSqlRecord r = db->selectRow("SELECT name, author, copyright, slideOrder, content FROM songs WHERE id = ?", {songId});

    QDomDocument dom("song");
    QDomElement rootElem = dom.createElement("song");
    dom.appendChild(rootElem);

    QDomElement titleElem = dom.createElement("title");
    titleElem.appendChild(dom.createTextNode(r.value("name").toString()));
    rootElem.appendChild(titleElem);

    QDomElement authorElem = dom.createElement("author");
    authorElem.appendChild(dom.createTextNode(r.value("author").toString()));
    rootElem.appendChild(authorElem);

    QDomElement copyrightElem = dom.createElement("copyright");
    copyrightElem.appendChild(dom.createTextNode(r.value("copyright").toString()));
    rootElem.appendChild(copyrightElem);

    QString content;
    for(const SongSectionWithContent &sswc : songSectionsWithContent(r.value("content").toString())) {
        content += QString("[%1]\n").arg(sswc.section.standardName());

        for(const QString &line : sswc.content.split("\n")) {
            ChordsInSong chords = songChords(line);
            if(chords.isEmpty()) {
                content += ' ';
                content += line;
                content += '\n';
                continue;
            }

            QString chordsLine = ".";
            QString lyricsLine = " ";

            int prevChordEnd = 0;
            for(const ChordInSong &chs : chords) {
                lyricsLine += line.mid(prevChordEnd, chs.annotationPos-prevChordEnd);

                // Prevent chords for sticking to each other (AmCdur)
                if(lyricsLine.length() <= chordsLine.length()) {
                    chordsLine += ' ';
                    lyricsLine += QString(chordsLine.length() - lyricsLine.length(), '_');
                }

                chordsLine += QString(lyricsLine.length()-chordsLine.length(), ' ');
                chordsLine += chs.chord.toString();

                prevChordEnd = chs.annotationPos + chs.annotationLength;
            }

            lyricsLine += line.mid(prevChordEnd);

            content += chordsLine;
            content += '\n';
            content += lyricsLine;
            content += '\n';
        }

        content += '\n';
    }

    content.remove('\r');

    QDomElement contentElem = dom.createElement("lyrics");
    contentElem.appendChild(dom.createTextNode(content));
    rootElem.appendChild(contentElem);

    QString filename = r.value("name").toString();
    if(!filename.length())
        filename = "__";

    static QRegularExpression invalidFilenameCharacterRegex("(?![., \\-_])\\W", QRegularExpression::UseUnicodePropertiesOption);
    filename.remove(invalidFilenameCharacterRegex);

    QFile f(outputDir.absoluteFilePath(filename));
    if(!f.open(QIODevice::WriteOnly)) {
        standardErrorDialog(tr("Nepodařilo se otevřít soubor \"%1\" pro zápis.").arg(filename));
        return false;
    }

    f.write(dom.toByteArray());

    return true;
}

void OpenSongExportDialog::on_btnClose_clicked()
{
    reject();
}

void OpenSongExportDialog::on_btnExport_clicked()
{
    QVector<qlonglong> songIds = ui->wgtSongSelection->selectedSongs();
    if(songIds.isEmpty())
        return standardErrorDialog(tr("Není vybrána žádná píseň k exportu."));

    QDir outputDirectory;
    {
        static const QIcon icon(":/icons/16/Export_16px.png");

        QFileDialog dlg(this);
        dlg.setFileMode(QFileDialog::Directory);
        dlg.setOption(QFileDialog::ShowDirsOnly);
        dlg.setAcceptMode(QFileDialog::AcceptOpen);
        dlg.setWindowIcon(icon);
        dlg.setWindowTitle(tr("Export"));
        dlg.setDirectory(settings->value("openSongExportDirectory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

        if(!dlg.exec())
            return;

        outputDirectory = dlg.directory();
        settings->setValue("openSongExportDirectory", outputDirectory.absolutePath());
    }

    const bool exportByTags = ui->cbFoldersByTags->isChecked();

    splashscreen->asyncAction(tr("Exportování písní"), true, [&]{
        for(int i = 0; i < songIds.size(); i++) {
            if(splashscreen->isStornoPressed())
                break;

            splashscreen->setProgress(i, songIds.size());

            const qlonglong songId = songIds[i];

            if(!exportByTags) {
                if(!exportSong(songId, outputDirectory)) {
                    splashscreen->storno();
                    break;
                }

            } else {
                QSqlQuery tagsQ = db->selectQuery("SELECT tag FROM song_tags WHERE song = ?", {songId});

                const auto exportToDir = [&](const QString &dirName) {
                    QDir d = outputDirectory;
                    d.mkdir(dirName);
                    if(!d.cd(dirName)) {
                        standardErrorDialog(tr("Nepodařilo se vytvořit složku '%1'.").arg(outputDirectory.absoluteFilePath(dirName)));
                        splashscreen->storno();
                        return false;
                    }

                    if(!exportSong(songId, d)) {
                        splashscreen->storno();
                        return false;
                    }

                    return true;
                };

                size_t tagCount = 0;
                while(tagsQ.next()) {
                    if(!exportToDir(tagsQ.value(0).toString()))
                        return;

                    tagCount ++;
                }

                if(!tagCount && !exportToDir(tr("Bez štítků")))
                    return;
            }
        }
    });

    if(!splashscreen->isStornoPressed())
        standardSuccessDialog(tr("Vybrané písně byly exportovány do \"%1\".").arg(outputDirectory.absolutePath()));
}
