#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QDate>
#include <QTime>
#include <QLocale>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

#include "gui/mainwindow.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	ui->lblVersion->setText(PROGRAM_VERSION);

	setFixedSize(minimumSize());

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

void AboutDialog::on_btnFacebookPage_clicked()
{
	QDesktopServices::openUrl(QUrl("https://www.facebook.com/strawLumen"));
}

void AboutDialog::on_btnWeb_clicked()
{
	QDesktopServices::openUrl(QUrl("https://straw-solutions.cz/?/portfolio/lumen"));
}

void AboutDialog::on_btnChangelog_clicked()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../changelog.html")));
}
