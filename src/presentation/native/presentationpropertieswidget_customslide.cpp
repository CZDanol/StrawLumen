#include "presentationpropertieswidget_customslide.h"
#include "ui_presentationpropertieswidget_customslide.h"
#include "presentation_customslide.h"

PresentationPropertiesWidget_CustomSlide::PresentationPropertiesWidget_CustomSlide(const QSharedPointer<Presentation_CustomSlide> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_CustomSlide),
	presentation_(presentation)
{
	ui->setupUi(this);

	connect(presentation.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(fillData()));
	connect(&textUpdateTimer_, SIGNAL(timeout()), this, SLOT(onUpdateTimerTimeout()));

	fillData();

	textUpdateTimer_.setSingleShot(true);
	textUpdateTimer_.setInterval(1000);
}

PresentationPropertiesWidget_CustomSlide::~PresentationPropertiesWidget_CustomSlide()
{
	if(textUpdateTimer_.isActive()) {
		onUpdateTimerTimeout();
		textUpdateTimer_.stop();
	}

	delete ui;
}

void PresentationPropertiesWidget_CustomSlide::fillData()
{
	ui->wgtStyle->setPresentationStyle(presentation_->style_);
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());

	ui->lnTitle->setText(presentation_->title_);
	ui->teText->setPlainText(presentation_->text_);
}

void PresentationPropertiesWidget_CustomSlide::onUpdateTimerTimeout()
{
	presentation_->text_ = ui->teText->toPlainText().trimmed();
	presentation_->updateDescription();
	emit presentation_->sigSlidesChanged();
}

void PresentationPropertiesWidget_CustomSlide::on_wgtStyle_sigPresentationStyleChangedByUser()
{
	presentation_->style_ = ui->wgtStyle->presentationStyle();
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
	// No need to emit
}

void PresentationPropertiesWidget_CustomSlide::on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &background)
{
	presentation_->style_.setBackground(background);
}

void PresentationPropertiesWidget_CustomSlide::on_lnTitle_editingFinished()
{
	presentation_->title_ = ui->lnTitle->text().trimmed();
	emit presentation_->sigItemChanged(presentation_.data());
}

void PresentationPropertiesWidget_CustomSlide::on_teText_textEdited()
{
	textUpdateTimer_.start();
}
