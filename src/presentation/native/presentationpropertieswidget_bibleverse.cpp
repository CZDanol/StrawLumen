#include "presentationpropertieswidget_bibleverse.h"
#include "ui_presentationpropertieswidget_bibleverse.h"

#include "presentation_bibleverse.h"
#include "presentation_bibleverse_wizard.h"
#include "presentation/native/presentation_customslide.h"
#include "util/biblerefsyntaxhighlighter.h"

PresentationPropertiesWidget_BibleVerse::PresentationPropertiesWidget_BibleVerse(const QSharedPointer<Presentation_BibleVerse> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_BibleVerse),
	presentation_(presentation)
{
	ui->setupUi(this);

	connect(presentation.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(fillData()));
	connect(&textUpdateTimer_, SIGNAL(timeout()), this, SLOT(onTextUpdateTimerTimeout()));

	fillData();

	textUpdateTimer_.setSingleShot(true);
	textUpdateTimer_.setInterval(1000);

	new BibleRefSyntaxHighlighter(ui->verses->document());
}

PresentationPropertiesWidget_BibleVerse::~PresentationPropertiesWidget_BibleVerse()
{
	if(textUpdateTimer_.isActive())
		onTextUpdateTimerTimeout();

	delete ui;
}

void PresentationPropertiesWidget_BibleVerse::fillData()
{
	ui->wgtStyle->setPresentationStyle(presentation_->style_);
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
	ui->verses->setPlainText(presentation_->versesStr());
}

void PresentationPropertiesWidget_BibleVerse::onTextUpdateTimerTimeout()
{
	presentation_->setVersesStr(ui->verses->toPlainText());

	emit presentation_->sigSlidesChanged();
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
	presentation_BibleVerse_wizard()->exec([&](const BibleRef &ref){
		ui->verses->appendPlainText(ref.toString());
	});
}

void PresentationPropertiesWidget_BibleVerse::on_verses_textChanged()
{
	textUpdateTimer_.start();
}

void PresentationPropertiesWidget_BibleVerse::on_btnMorphIntoCustomSlide_clicked()
{
	emit presentation_->sigMorphedInto(presentation_, presentation_->toCustomSlide());
}
