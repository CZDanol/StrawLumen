#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow_PresentationMode;
class MainWindow_SongsMode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public:
	void editSong(qlonglong songId);

public:
	void showPresentationMode();
	void blinkPresentationModeButton();
	MainWindow_PresentationMode *presentationMode();

	void showSongsMode();
	MainWindow_SongsMode *songsMode();

	QVector<qlonglong> selectedSongIds();

signals:
	void sigClosed();

public slots:
	void onDbQueryError(const QString &query, const QString &error);
	void onDbDatabaseError(const QString &error);

protected:
	void closeEvent(QCloseEvent *e) override;
	void dragEnterEvent(QDragEnterEvent *e) override;
	void dropEvent(QDropEvent *e) override;

private slots:
	void on_actionSettings_triggered();
	void on_btnPresentationMode_clicked();
	void on_btnSongsMode_clicked();
	void on_actionBackgrounds_triggered();
	void on_actionStyles_triggered();
	void on_actionGenerateDocuments_triggered();
	void on_actionAbout_triggered();
	void on_actionSendFeedback_triggered();
	void on_actionBulkEditSongs_triggered();
	void on_actionBibleMgmt_triggered();

private:
	Ui::MainWindow *ui;

};

extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
