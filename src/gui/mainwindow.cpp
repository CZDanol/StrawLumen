#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "util/standarddialogs.h"
#include "gui/settingsdialog.h"
#include "gui/projectorwindow.h"
#include "job/db.h"
#include "job/settings.h"

MainWindow *mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->swMenu->addWidget(ui->wgtPresentationMode->menuWidget());
	ui->swMenu->addWidget(ui->wgtSongsMode->menuWidget());

	ui->btnPresentationMode->click();

	setWindowTitle(tr("Straw Lumen v%1").arg(PROGRAM_VERSION));
	restoreGeometry(settings->value("window.mainWindow.geometry").toByteArray());

	connect(db, SIGNAL(sigQueryError(QString,QString)), this, SLOT(onDbQueryError(QString,QString)));
}

MainWindow::~MainWindow()
{
	settings->setValue("window.mainWindow.geometry", saveGeometry());

	delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	projectorWindow->close();
	QMainWindow::closeEvent(e);
}

void MainWindow::onDbQueryError(const QString &error, const QString &sql)
{
	standardErrorDialog(tr("Chyba databáze: %1\n\n%2").arg(error, sql));
}

void MainWindow::on_actionSettings_triggered()
{
	settingsDialog->show();
}

void MainWindow::on_btnPresentationMode_clicked()
{
	ui->swModes->setCurrentWidget(ui->wgtPresentationMode);
	ui->swMenu->setCurrentWidget(ui->wgtPresentationMode->menuWidget());
}

void MainWindow::on_btnSongsMode_clicked()
{
	ui->swModes->setCurrentWidget(ui->wgtSongsMode);
	ui->swMenu->setCurrentWidget(ui->wgtSongsMode->menuWidget());
}
