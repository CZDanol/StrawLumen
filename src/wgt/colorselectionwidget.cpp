#include "colorselectionwidget.h"
#include "ui_colorselectionwidget.h"

#include <QStyle>

ColorSelectionWidget::ColorSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorSelectionWidget)
{
	ui->setupUi(this);
	ui->wgtC1->setComponent(ColorComponentWidget::ccHslHue);
	ui->wgtC2->setComponent(ColorComponentWidget::ccHslSaturation);
	ui->wgtC3->setComponent(ColorComponentWidget::ccLightness);
	ui->wgtCAlpha->setComponent(ColorComponentWidget::ccAlpha);

	connect(ui->wgtC1, SIGNAL(sigColorChangedByUser(QColor)), this, SLOT(onComponentColorChangedByUser(QColor)));
	connect(ui->wgtC2, SIGNAL(sigColorChangedByUser(QColor)), this, SLOT(onComponentColorChangedByUser(QColor)));
	connect(ui->wgtC3, SIGNAL(sigColorChangedByUser(QColor)), this, SLOT(onComponentColorChangedByUser(QColor)));
	connect(ui->wgtCAlpha, SIGNAL(sigColorChangedByUser(QColor)), this, SLOT(onComponentColorChangedByUser(QColor)));
}

ColorSelectionWidget::~ColorSelectionWidget()
{
	delete ui;
}

void ColorSelectionWidget::setColor(const QColor &color)
{
	// Cannot be because comparison converts to RGB -> in some cases saturation information can be lost
	/*if(currentColor_ == color)
		return;*/

	currentColor_ = color;

	ui->wgtC1->setColor(color);
	ui->wgtC2->setColor(color);
	ui->wgtC3->setColor(color);
	ui->wgtCAlpha->setColor(color);

	ui->lnHex->setText(color.name(isAlphaChannelEnabled_ ? QColor::HexArgb : QColor::HexRgb));
}

void ColorSelectionWidget::setAlphaChannelEnabled(bool set)
{
	if(isAlphaChannelEnabled_ == set)
		return;

	isAlphaChannelEnabled_ = set;
	ui->wgtCAlpha->setVisible(set);
}

void ColorSelectionWidget::setReadOnly(bool set)
{
	if(isReadOnly_ == set)
		return;

	isReadOnly_ = set;

	ui->lnHex->setReadOnly(set);
	ui->lnHex->style()->unpolish(ui->lnHex);
	ui->lnHex->style()->polish(ui->lnHex);

	ui->wgtC1->setEnabled(!set);
	ui->wgtC2->setEnabled(!set);
	ui->wgtC3->setEnabled(!set);
	ui->wgtCAlpha->setEnabled(!set);
}

bool ColorSelectionWidget::isAlphaChannelEnabled() const
{
	return isAlphaChannelEnabled_;
}

void ColorSelectionWidget::onComponentColorChangedByUser(const QColor &newColor)
{
	setColor(newColor);
	emit sigColorChangedByUser(newColor);
}

void ColorSelectionWidget::on_lnHex_editingFinished()
{
	setColor(QColor(ui->lnHex->text()));
	emit sigColorChangedByUser(currentColor_);
}
