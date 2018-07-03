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

protected:
	void closeEvent(QCloseEvent *e) override;

private slots:
	void on_actionSettings_triggered();

private:
	Ui::MainWindow *ui;

};

extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
