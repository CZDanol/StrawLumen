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
	ui->cbBlackSlideBefore->setChecked(presentation->blackSlideBefore_);
	ui->cbBlackSlideAfter->setChecked(presentation_->blackSlideAfter_);

	updateUiEnabled();
}

PresentationPropertiesWidget_PowerPoint::~PresentationPropertiesWidget_PowerPoint()
{
	delete ui;
}

void PresentationPropertiesWidget_PowerPoint::updateUiEnabled()
{
	const bool isAutoPresentation = ui->cbAutoPresentation->isChecked();

	ui->cbBlackSlideBefore->setEnabled(!isAutoPresentation);
	ui->cbBlackSlideAfter->setEnabled(!isAutoPresentation);
}

void PresentationPropertiesWidget_PowerPoint::on_cbAutoPresentation_clicked(bool checked)
{
	presentation_->isAutoPresentation_ = checked;
	updateUiEnabled();

	emit presentation_->sigItemChanged(presentation_.data());
	emit presentation_->sigSlidesChanged();

	if(presentationManager->currentPresentation() == presentation_)
		presentationManager->reinitializeCurrentPresentation();
}

void PresentationPropertiesWidget_PowerPoint::on_cbBlackSlideBefore_clicked(bool checked)
{
	presentation_->blackSlideBefore_ = checked;
	emit presentation_->sigSlidesChanged();
}

void PresentationPropertiesWidget_PowerPoint::on_cbBlackSlideAfter_clicked(bool checked)
{
	presentation_->blackSlideAfter_ = checked;
	emit presentation_->sigSlidesChanged();
}
