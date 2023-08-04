#ifndef MAINWINDOW_SONGSMODE_H
#define MAINWINDOW_SONGSMODE_H

#include <QWidget>
#include <QMenu>
#include <QCompleter>
#include <QStringList>
#include <QStringListModel>
#include <QRegularExpressionValidator>
#include <QTextCursor>

#include "rec/songsection.h"

namespace Ui {
    class MainWindow_SongsMode;
}

class MainWindow_SongsMode : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow_SongsMode(QWidget *parent = nullptr);
    ~MainWindow_SongsMode();

public:
    QWidget *menuWidget();
    QMenu *importMenu();
    QMenu *exportMenu();

public slots:
    void editSong(qlonglong songId);
    void requestDeleteSongs(const QVector<qlonglong> &songIds);

    /// Returns false if the user does not wish do close the edit mode
    bool askFinishEditMode();

    QVector<qlonglong> selectedSongIds();

private:
    void setSongEditMode(bool set);

    void updateSongManipulationButtonsEnabled();
    void updateCopyChordsMenu();
    void updateTeContentMenu();
    void updateMoveChordActionsEnabled();

    void insertSongSection(const SongSection &section, bool positionCursorInMiddle = false);
    void contentSelectionMorph(const std::function<QString(QString)> &callback, bool onlySection);
    void moveChords(bool right);

    /// Returns position and content of a song section around position pos
    QPair<int,QString> songSectionAroundPos(int pos);

private slots:
    void fillSongData();

private slots:
    void onCurrentSongChanged(qlonglong songId, int prevRowId);
    void onSelectionChanged();
    void onSongListContextMenuRequested(const QPoint &globalPos);
    void onTeContentContextMenuRequested(const QPoint &pos);

private slots:
    void on_btnNew_clicked();
    void on_btnDiscardChanges_clicked();
    void on_btnSaveChanges_clicked();
    void on_btnEdit_clicked();
    void on_actionDeleteSongs_triggered();
    void on_lnSlideOrder_sigFocused();
    void on_btnInsertChord_clicked();
    void on_btnTransposeUp_clicked();
    void on_btnTransposeDown_clicked();
    void on_btnAddCustomSlideOrderItem_pressed();
    void on_btnInsertSlideSeparator_clicked();
    void on_actionImportOpenSongSong_triggered();
    void on_btnCreateSongbook_clicked();
    void on_actionCreateSongbookFromSelection_triggered();
    void on_actionExportToLumen_triggered();
    void on_actionImportFromLumen_triggered();
    void on_actionPresentSongs_triggered();
    void on_actionAddSongsToPlaylist_triggered();
    void on_actionExportToOpenSong_triggered();
    void on_btnCopyChords_pressed();
    void on_actionDeleteChords_triggered();
    void on_btnAutoFormat_clicked();
    void on_actionMoveChordsRight_triggered();
    void on_teContent_cursorPositionChanged();
    void on_actionMoveChordsLeft_triggered();
    void on_actionOnlyChords_triggered();
    void on_btnBulkEdit_clicked();
    void on_btnConvertChords_clicked();
    void on_btnTransposeFlat_clicked();
    void on_actionImportFromPowerPoint_triggered();

private:
    Ui::MainWindow_SongsMode *ui;
    QMenu insertSectionMenu_, importMenu_, exportMenu_, songListContextMenu_, copyChordsMenu_, teContentMenu_;

private:
    QRegularExpressionValidator slideOrderValidator_;
    QCompleter *slideOrderCompleter_;
    QStringListModel slideOrderCompleterModel_;
    QMenu addCustomSlideOrderItemMenu_;

private:
    qlonglong currentSongId_ = -1;
    bool isSongEditMode_ = false;

};

#endif // MAINWINDOW_SONGSMODE_H
