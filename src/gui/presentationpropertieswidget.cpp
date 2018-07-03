#include "presentationpropertieswidget.h"
#include "ui_presentationpropertieswidget.h"

#include "presentation/presentation.h"

PresentationPropertiesWidget::PresentationPropertiesWidget(const QSharedPointer<Presentation> &presentation, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PresentationPropertiesWidget),
	presentation_(presentation)
{
	ui->setupUi(this);

	ui->lblIcon->setPixmap(presentation->icon());
	ui->lblTitle->setText(presentation->identification());

	propertiesWidget_ = presentation->createPropertiesWidget(this);
	ui->ltMain->insertWidget(2, propertiesWidget_);
}

PresentationPropertiesWidget::~PresentationPropertiesWidget()
{
	delete ui;
}
