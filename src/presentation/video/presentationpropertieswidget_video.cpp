#include "presentationpropertieswidget_video.h"
#include "ui_presentationpropertieswidget_video.h"

#include "presentation/presentationmanager.h"
#include "presentation_video.h"
#include "videoprojectorwindow.h"

PresentationPropertiesWidget_Video::PresentationPropertiesWidget_Video(const QSharedPointer<Presentation_Video> &presentation, QWidget *parent) : QWidget(parent),
                                                                                                                                                  ui(new Ui::PresentationPropertiesWidget_Video),
                                                                                                                                                  presentation_(presentation) {
	ui->setupUi(this);

	fillData();
}

PresentationPropertiesWidget_Video::~PresentationPropertiesWidget_Video() {
	delete ui;
}

void PresentationPropertiesWidget_Video::fillData() {
	ui->cbRepeat->setChecked(presentation_->repeat_);
	ui->cbAutoPlay->setChecked(presentation_->autoPlay_);
}

void PresentationPropertiesWidget_Video::on_cbAutoPlay_clicked(bool checked) {
	presentation_->autoPlay_ = checked;
	emit presentation_->sigChanged();
}

void PresentationPropertiesWidget_Video::on_cbRepeat_clicked(bool checked) {
	presentation_->repeat_ = checked;
	emit presentation_->sigItemChanged(presentation_.data());

	if(presentationManager->currentPresentation() == presentation_)
		videoProjectorWindow->setRepeat(checked);
}
