#include "presentationpropertieswidget_powerpoint.h"
#include "ui_presentationpropertieswidget_powerpoint.h"

#include "presentation/powerpoint/presentation_powerpoint.h"
#include "presentation/presentationmanager.h"

PresentationPropertiesWidget_PowerPoint::PresentationPropertiesWidget_PowerPoint(const QSharedPointer<Presentation_PowerPoint> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_PowerPoint),
	presentation_(presentation)
{
	ui->setupUi(this);

	ui->cbAutoPresentation->setChecked(presentation->isAutoPresentation_);
}

PresentationPropertiesWidget_PowerPoint::~PresentationPropertiesWidget_PowerPoint()
{
	delete ui;
}

void PresentationPropertiesWidget_PowerPoint::on_cbAutoPresentation_clicked(bool checked)
{
	presentation_->isAutoPresentation_ = checked;

	emit presentation_->sigItemChanged(presentation_.data());
	emit presentation_->sigSlidesChanged();

	if(presentationManager->currentPresentation() == presentation_)
		presentationManager->reinitializeCurrentPresentation();
}
