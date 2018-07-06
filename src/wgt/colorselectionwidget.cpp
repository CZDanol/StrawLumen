#include "colorselectionwidget.h"
#include "ui_colorselectionwidget.h"

ColorSelectionWidget::ColorSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorSelectionWidget)
{
	ui->setupUi(this);
	ui->wgtC1->setComponent(ColorComponentWidget::ccHue);
	ui->wgtC2->setComponent(ColorComponentWidget::ccSaturation);
	ui->wgtC3->setComponent(ColorComponentWidget::ccValue);

	connect(ui->wgtC1, SIGNAL(sigColorChanged(QColor)), this, SLOT(setColor(QColor)));
	connect(ui->wgtC2, SIGNAL(sigColorChanged(QColor)), this, SLOT(setColor(QColor)));
	connect(ui->wgtC3, SIGNAL(sigColorChanged(QColor)), this, SLOT(setColor(QColor)));
}

ColorSelectionWidget::~ColorSelectionWidget()
{
	delete ui;
}

void ColorSelectionWidget::setColor(const QColor &color)
{
	if(currentColor_ == color)
		return;

	currentColor_ = color;

	ui->wgtC1->setColor(color);
	ui->wgtC2->setColor(color);
	ui->wgtC3->setColor(color);

	emit sigColorChanged(color);
}
