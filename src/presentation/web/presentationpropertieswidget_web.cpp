#include "presentationpropertieswidget_web.h"
#include "ui_presentationpropertieswidget_web.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "presentation_web.h"
#include "presentation/presentationmanager.h"
#include "presentation/web/webprojectorwindow.h"
#include "job/settings.h"

PresentationPropertiesWidget_Web::PresentationPropertiesWidget_Web(const QSharedPointer<Presentation_Web> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_Web),
	presentation_(presentation)
{
	ui->setupUi(this);
	fillData();
}

PresentationPropertiesWidget_Web::~PresentationPropertiesWidget_Web()
{
	on_lnUrl_editingFinished();

	delete ui;
}

void PresentationPropertiesWidget_Web::fillData()
{
	ui->lnUrl->setText(presentation_->url_.toString());
}

void PresentationPropertiesWidget_Web::on_lnUrl_editingFinished()
{
	const QUrl newUrl = QUrl::fromUserInput(ui->lnUrl->text());

	if(presentation_->url_ == newUrl)
		return;

	presentation_->url_ = newUrl;
	presentation_->updateIdentification();
	emit presentation_->sigItemChanged(presentation_.data());

	if(presentationManager->currentPresentation() == presentation_)
		webProjectorWindow->setUrl(presentation_->url_);
}

void PresentationPropertiesWidget_Web::on_btnLocalFile_clicked()
{
	static const QIcon icon(":/icons/16/Open_16px.png");

	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setAcceptMode(QFileDialog::AcceptOpen);
	dlg.setNameFilter(tr("Webová stránka (%1)").arg("*." + Presentation_Web::validExtensions().join(" *.")));
	dlg.setWindowIcon(icon);
	dlg.setWindowTitle(tr("Lokální webová stránka"));
	dlg.setDirectory(settings->value("webDirectory", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString());

	if(!dlg.exec())
		return;

	settings->setValue("webDirectory", dlg.directory().absolutePath());

	ui->lnUrl->setText(dlg.selectedUrls().first().toString());
	on_lnUrl_editingFinished();
}
