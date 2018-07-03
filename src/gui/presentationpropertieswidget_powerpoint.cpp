#include "presentationpropertieswidget_powerpoint.h"
#include "ui_presentationpropertieswidget_powerpoint.h"

#include "presentation/presentation_powerpoint.h"
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
	if(checked) {
		presentation_->slideOrder_.resize(1);
		presentation_->slideOrder_[0] = -1;
	} else
		presentation_->initDefaultSlideOrder();

	emit presentation_->sigSlidesChanged();

	if(presentationManager->currentPresentation() == presentation_)
		presentationManager->reinitializeCurrentPresentation();
}
