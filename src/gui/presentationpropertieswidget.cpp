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
	if(propertiesWidget_)
		ui->ltMain->insertWidget(1, propertiesWidget_);
}

PresentationPropertiesWidget::~PresentationPropertiesWidget()
{
	delete ui;
}

QSharedPointer<Presentation> PresentationPropertiesWidget::presentation() const
{
	return presentation_;
}
