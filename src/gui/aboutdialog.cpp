#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDate>
#include <QTime>
#include <QLocale>

#include "gui/mainwindow.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	ui->lblVersion->setText(PROGRAM_VERSION);

	QLocale locale( QLocale::English );
	ui->lblBuildDate->setText(QDateTime(locale.toDate(__DATE__, "MMM dd yyyy"), QTime::fromString(__TIME__)).toString("dd.MM.yyyy HH:mm"));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

AboutDialog *aboutDialog()
{
	static AboutDialog *dlg = nullptr;
	if(!dlg)
		dlg = new AboutDialog(mainWindow);

	return dlg;
}
