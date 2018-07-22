#include "presentationpropertieswidget_song.h"
#include "ui_presentationpropertieswidget_song.h"

#include "presentation/native/presentation_song.h"
#include "gui/mainwindow.h"
#include "gui/mainwindow_songsmode.h"

PresentationPropertiesWidget_Song::PresentationPropertiesWidget_Song(const QSharedPointer<Presentation_Song> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget_Song),
	presentation_(presentation)
{
	ui->setupUi(this);
	connect(presentation.data(), SIGNAL(sigItemChanged(Presentation*)), this, SLOT(fillData()));
	connect(ui->wgtStyle, SIGNAL(sigPresentationStyleChangedByUser()), this, SLOT(onStyleChangedByUser()));

	connect(&presentation->style_, SIGNAL(sigChanged()), this, SLOT(onPresentationStyleChanged()));

	// Slide order
	{
		slideOrderCompleter_.setModel(&slideOrderCompleterModel_);
		slideOrderCompleter_.setCaseSensitivity(Qt::CaseInsensitive);

		slideOrderValidator_.setRegularExpression(songCustomSlideOrderRegex());

		ui->lnSlideOrder->setCompleter(&slideOrderCompleter_);
		ui->lnSlideOrder->setValidator(&slideOrderValidator_);

		addCustomSlideOrderItemMenu_ = new QMenu(this);
		ui->btnAddSlideOrderItem->setMenu(addCustomSlideOrderItemMenu_);
	}

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

	ui->lnSlideOrder->setText(presentation_->customSlideOrder_);

	ui->cbEmptySlideBefore->setChecked(presentation_->emptySlideBefore_);
	ui->cbEmptySlideAfter->setChecked(presentation_->emptySlideAfter_);
	ui->cbIgnoreEmptySlides->setChecked(presentation_->emptySlideAfter_);

	ui->wgtStylePreview->setPresentationStyle(presentation_->style_);
}

void PresentationPropertiesWidget_Song::onStyleChangedByUser()
{
	presentation_->style_ = ui->wgtStyle->presentationStyle();
	ui->wgtBackground->setPresentationBackground(presentation_->style_.background());
	// No need to emit
}

void PresentationPropertiesWidget_Song::onPresentationStyleChanged()
{
	ui->wgtStylePreview->setPresentationStyle(presentation_->style_);
}

void PresentationPropertiesWidget_Song::on_wgtBackground_sigPresentationBackgroundChangedByUser(const PresentationBackground &background)
{
	presentation_->style_.setBackground(background);
}

void PresentationPropertiesWidget_Song::on_lnSlideOrder_editingFinished()
{
	presentation_->customSlideOrder_ = ui->lnSlideOrder->text();
	presentation_->loadSlideOrder();
}

void PresentationPropertiesWidget_Song::on_lnSlideOrder_sigFocused()
{
	QStringList lst;
	for(const Presentation_Song::SectionRec &sr : presentation_->sections_)
		lst.append(sr.section.standardName());

	slideOrderCompleterModel_.setStringList(lst);
}

void PresentationPropertiesWidget_Song::on_btnAddSlideOrderItem_pressed()
{
	addCustomSlideOrderItemMenu_->clear();

	for(const Presentation_Song::SectionRec &sr : presentation_->sections_) {
		SongSection section = sr.section;
		addCustomSlideOrderItemMenu_->addAction(sr.section.icon(), sr.section.userFriendlyName(), [this, section]{
			const QString text = ui->lnSlideOrder->text();
			ui->lnSlideOrder->setText((text.isEmpty() ? "" : text + " ") + section.standardName());
			on_lnSlideOrder_editingFinished();
		});
	}
}

void PresentationPropertiesWidget_Song::on_cbEmptySlideBefore_clicked(bool checked)
{
	presentation_->emptySlideBefore_ = checked;
	presentation_->loadSlideOrder();
}

void PresentationPropertiesWidget_Song::on_cbEmptySlideAfter_clicked(bool checked)
{
	presentation_->emptySlideAfter_ = checked;
	presentation_->loadSlideOrder();
}

void PresentationPropertiesWidget_Song::on_btnEditSong_clicked()
{
	mainWindow->showSongsMode();
	mainWindow->songsMode()->editSong(presentation_->songId_);
}

void PresentationPropertiesWidget_Song::on_cbIgnoreEmptySlides_clicked(bool checked)
{
	presentation_->ignoreEmptySlides_ = checked;
	presentation_->loadSlideOrder();
}
