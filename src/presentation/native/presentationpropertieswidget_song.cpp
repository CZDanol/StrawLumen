#include "presentationpropertieswidget_song.h"
#include "ui_presentationpropertieswidget_song.h"

#include "presentation/native/presentation_song.h"

PresentationPropertiesWidget_Song::PresentationPropertiesWidget_Song(const QSharedPointer<Presentation_Song> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_Song),
	presentation_(presentation)
{
	ui->setupUi(this);
	connect(presentation.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(fillData()));
	connect(ui->wgtStyle, SIGNAL(sigPresentationStyleChangedByUser()), this, SLOT(onStyleChangedByUser()));

	fillData();
}

PresentationPropertiesWidget_Song::~PresentationPropertiesWidget_Song()
{
	delete ui;
}

void PresentationPropertiesWidget_Song::fillData()
{
	ui->wgtStyle->setPresentationStyle(presentation_->style_);
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
}

void PresentationPropertiesWidget_Song::onStyleChangedByUser()
{
	presentation_->style_ = ui->wgtStyle->presentationStyle();
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
	// No need to emit
}

void PresentationPropertiesWidget_Song::on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &background)
{
	presentation_->style_.setBackground(background);
}
