#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public:
	void editSong(qlonglong songId);

protected:
	void closeEvent(QCloseEvent *e) override;

private slots:
	void onDbQueryError(const QString &error, const QString &sql);

private slots:
	void on_actionSettings_triggered();
	void on_btnPresentationMode_clicked();
	void on_btnSongsMode_clicked();

private:
	Ui::MainWindow *ui;

};

extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
