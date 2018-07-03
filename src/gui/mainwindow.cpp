#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "gui/settingsdialog.h"
#include "gui/projectorwindow.h"

MainWindow *mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->swMenu->addWidget(ui->wgtPresentationMode->menuWidget());

	setWindowTitle(tr("Straw Lumen v%1").arg(PROGRAM_VERSION));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	projectorWindow->close();
	QMainWindow::closeEvent(e);
}

void MainWindow::on_actionSettings_triggered()
{
	settingsDialog->show();
}
