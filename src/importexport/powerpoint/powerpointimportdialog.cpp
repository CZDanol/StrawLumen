#include "powerpointimportdialog.h"
#include "gui/mainwindow_presentationmode.h"
#include "rec/playlist.h"
#include "ui_powerpointimportdialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>

#include "job/settings.h"
#include "presentation/powerpoint/presentation_powerpoint.h"
#include "presentation/native/presentation_song.h"
#include "job/db.h"
#include "gui/splashscreen.h"
#include "util/standarddialogs.h"
#include "gui/mainwindow.h"

enum ConflictBehavior {
    cbSkip,
    cbOverwriteIfNewer,
    cbAlwaysOverwrite
};

PowerPointImportDialog::PowerPointImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerPointImportDialog)
{
    ui->setupUi(this);
}

PowerPointImportDialog::~PowerPointImportDialog()
{
    delete ui;
}

PowerPointImportDialog *PowerPointImportDialog::instance()
{
    static PowerPointImportDialog *dlg = new PowerPointImportDialog();
    return dlg;
}

void PowerPointImportDialog::show()
{
    inputFiles_.clear();
    updateUi();
    QDialog::show();
}

void PowerPointImportDialog::updateUi()
{
    ui->btnSelectFile->setText(inputFiles_.isEmpty() ? tr("Vybrat soubory") : tr("%1 souborů").arg(inputFiles_.size()));
    ui->btnImport->setEnabled(!inputFiles_.isEmpty());
}

void PowerPointImportDialog::on_btnImport_clicked()
{
    const int conflictBehavior = ui->cmbConflictBehavior->currentIndex();
    const bool addToPlaylist = ui->cbAddToPlaylist->isChecked();
    const QSet<QString> tags = ui->lnTags->toTags();

    bool isError = false;
    QVector<QSharedPointer<Presentation> > presentations;

    splashscreen->asyncAction(tr("Impportování písní"), true, [&]{
        db->beginTransaction();

        int i = 0;
        for(const QString &file : inputFiles_) {
            if(isError || splashscreen->isStornoPressed())
                break;

            splashscreen->setProgress(i++, inputFiles_.size());

            QFileInfo fi(file);
            if(!fi.exists())
                continue;

            const QString uid = QStringLiteral("ppt_%1_%2").arg(fi.fileName(), QString::number(fi.birthTime().toSecsSinceEpoch()));
            const qlonglong fileLastEdit = fi.lastModified().toSecsSinceEpoch();

            const QSqlRecord existingSong = db->selectRowDef("SELECT id, lastEdit FROM songs WHERE uid = ?", {uid});
            qlonglong songId = existingSong.isEmpty() ? -1 : existingSong.value("id").toLongLong();

            static const QStringList dataFields {"name", "author", "copyright", "content", "slideOrder", "notes", "lastEdit"};
            bool skip = false;

            if(songId != -1 && (conflictBehavior == cbSkip || (conflictBehavior == cbOverwriteIfNewer && fileLastEdit <= existingSong.value("lastEdit").toLongLong())))
                skip = true;

            if(!skip) {
                QSharedPointer<Presentation_PowerPoint> ppt = Presentation_PowerPoint::createFromFilename(fi.absoluteFilePath(), false, true);

                QStringList content;
                for(const QSharedPointer<Presentation_PowerPoint::Slide> &slide : ppt->getSlides())
                    content += QStringLiteral("{V%1}\n%2").arg(QString::number(content.size() + 1), slide->text.trimmed());

                QHash<QString,QVariant> data {
                    {"uid", uid},
                    {"name", fi.completeBaseName()},
                    {"lastEdit", fileLastEdit},
                    {"author", ""},
                    {"copyright", ""},
                    {"content", content.join("\n\n")},
                    {"notes", ""},
                    {"slideOrder", ""},
                };

                if(songId == -1)
                    songId = db->insert("songs", data).toLongLong();
                else
                    db->update("songs", data, "id = ?", {songId});

								DatabaseManager::updateSongFulltextIndex(db, songId);
						}

            for(const QString &tag : tags)
                db->exec("INSERT OR IGNORE INTO song_tags(song, tag) VALUES(?, ?)", {songId, tag});

            if(addToPlaylist)
                presentations.append(Presentation_Song::createFromDb(songId));
        }

        db->commitTransaction();
    });

    emit db->sigSongListChanged();

    if(!presentations.isEmpty() && (
             presentations.size() < 20
             || standardConfirmDialog(tr("Importovaných písní je mnoho (%1). Opravdu je chcete všechny přidat do programu promítání?").arg(presentations.size()))
             )) {
            mainWindow->presentationMode()->playlist()->addItems(presentations);
    }

    if(!isError) {
        if(!splashscreen->isStornoPressed())
            standardSuccessDialog(tr("Písně byly importovány."));

        close();
    }
}

void PowerPointImportDialog::on_btnClose_clicked()
{
    reject();
}

void PowerPointImportDialog::on_btnSelectFile_clicked()
{
    static const QIcon icon(":/icons/16/Import_16px.png");

    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setNameFilter(tr("Soubory PowerPoint (*.ppt *.pptx)"));
    dlg.setWindowIcon(icon);
    dlg.setWindowTitle(tr("Import"));
    dlg.setDirectory(settings->value("powerPointImportDirectory", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString());

    if(!dlg.exec())
        return;

    settings->setValue("powerPointImportDirectory", dlg.directory().absolutePath());
    inputFiles_ = dlg.selectedFiles();

    updateUi();
}
