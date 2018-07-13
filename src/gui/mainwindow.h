#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow_PresentationMode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public:
	void editSong(qlonglong songId);

public:
	MainWindow_PresentationMode *presentationMode();

public slots:
	void onDbQueryError(const QString &query, const QString &error);
	void onDbDatabaseError(const QString &error);

protected:
	void closeEvent(QCloseEvent *e) override;

private slots:
	void on_actionSettings_triggered();
	void on_btnPresentationMode_clicked();
	void on_btnSongsMode_clicked();
	void on_actionBackgrounds_triggered();
	void on_actionStyles_triggered();
	void on_actionGenerateDocuments_triggered();
	void on_actionAbout_triggered();

private:
	Ui::MainWindow *ui;

};

extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
