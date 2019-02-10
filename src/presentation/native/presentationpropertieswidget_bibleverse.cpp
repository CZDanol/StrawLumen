#include "presentationpropertieswidget_bibleverse.h"
#include "ui_presentationpropertieswidget_bibleverse.h"

#include "presentation_bibleverse.h"
#include "presentation_bibleverse_wizard.h"

PresentationPropertiesWidget_BibleVerse::PresentationPropertiesWidget_BibleVerse(const QSharedPointer<Presentation_BibleVerse> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_BibleVerse),
	presentation_(presentation)
{
	ui->setupUi(this);

	connect(presentation.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(fillData()));

	fillData();
}

PresentationPropertiesWidget_BibleVerse::~PresentationPropertiesWidget_BibleVerse()
{
	delete ui;
}

void PresentationPropertiesWidget_BibleVerse::fillData()
{
	ui->wgtStyle->setPresentationStyle(presentation_->style_);
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
}

void PresentationPropertiesWidget_BibleVerse::on_wgtStyle_sigPresentationStyleChangedByUser()
{
	presentation_->style_ = ui->wgtStyle->presentationStyle();
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
	// No need to emit
}

void PresentationPropertiesWidget_BibleVerse::on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &background)
{
	presentation_->style_.setBackground(background);
}

void PresentationPropertiesWidget_BibleVerse::on_btnWizard_clicked()
{
	presentation_BibleVerse_wizard()->show();
}
