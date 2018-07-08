#include "presentationbackgroundselectionwidget.h"
#include "ui_presentationbackgroundselectionwidget.h"

#include "gui/backgrounddialog.h"

#include <QDebug>

PresentationBackgroundSelectionWidget::PresentationBackgroundSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationBackgroundSelectionWidget)
{
	ui->setupUi(this);
}

PresentationBackgroundSelectionWidget::~PresentationBackgroundSelectionWidget()
{
	delete ui;
}

void PresentationBackgroundSelectionWidget::setPresentationBackground(const PresentationBackground &background)
{
	presentationBackground_ = background;
	updateLabel();
}

void PresentationBackgroundSelectionWidget::setReadOnly(bool set)
{
	if(isReadOnly_ == set)
		return;

	isReadOnly_ = set;
	ui->btnSelect->setEnabled(!set);
}

void PresentationBackgroundSelectionWidget::updateLabel()
{
	ui->lblLabel->setText(presentationBackground_.caption());
}

void PresentationBackgroundSelectionWidget::on_btnSelect_clicked()
{
	presentationBackground_ = backgroundDialog->showInSelectionMode(presentationBackground_);
	emit sigPresentationBackgroundChangedByUser(presentationBackground_);
}
