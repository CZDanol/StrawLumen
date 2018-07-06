#include "textstylewidget.h"
#include "ui_textstylewidget.h"

TextStyleWidget::TextStyleWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TextStyleWidget)
{
	ui->setupUi(this);

	connect(this, SIGNAL(sigTextStyleChanged(TextStyle)), ui->wgtPreview, SLOT(setTextStyle(TextStyle)));

	forceSetTextStyle(textStyle_);
}

TextStyleWidget::~TextStyleWidget()
{
	delete ui;
}

const TextStyle &TextStyleWidget::textStyle() const
{
	return textStyle_;
}

void TextStyleWidget::setTextStyle(const TextStyle &style)
{
	if(textStyle_ == style)
		return;

	forceSetTextStyle(style);
}

void TextStyleWidget::forceSetTextStyle(const TextStyle &style)
{
	textStyle_ = style;
	isSettingUp_ = true;

	ui->wgtPreview->setTextStyle(style);
	ui->wgtFont->setSelectedFont(style.font);
	ui->wgtColor->setColor(style.color);

	ui->cbOutline->setChecked(style.outlineEnabled);
	ui->wgtOutlineColor->setColor(style.outlineColor);
	ui->sbOutlineWidth->setValue(style.outlineWidth);

	ui->wgtPreview->forceSetTextStyle(textStyle_);

	isSettingUp_ = false;
}

void TextStyleWidget::on_wgtFont_sigFontChanged(const QFont &font)
{
	textStyle_.font = font;
	emit sigTextStyleChanged(textStyle_);
}

void TextStyleWidget::on_wgtColor_sigColorChanged(const QColor &color)
{
	textStyle_.color = color;
	emit sigTextStyleChanged(textStyle_);
}

void TextStyleWidget::on_cbOutline_clicked(bool checked)
{
	textStyle_.outlineEnabled = checked;
	emit sigTextStyleChanged(textStyle_);
}

void TextStyleWidget::on_sbOutlineWidth_valueChanged(int arg1)
{
	textStyle_.outlineWidth = arg1;

	if(!isSettingUp_) {
		ui->cbOutline->setChecked(true);
		textStyle_.outlineEnabled = true;

		emit sigTextStyleChanged(textStyle_);
	}
}

void TextStyleWidget::on_wgtOutlineColor_sigColorChanged(const QColor &color)
{
	textStyle_.outlineColor = color;

	if(!isSettingUp_) {
		ui->cbOutline->setChecked(true);
		textStyle_.outlineEnabled = true;

		emit sigTextStyleChanged(textStyle_);
	}
}

void TextStyleWidget::on_cbBackground_clicked(bool checked)
{
	textStyle_.backgroundEnabled = checked;
	emit sigTextStyleChanged(textStyle_);
}

void TextStyleWidget::on_cbBackgroundPadding_valueChanged(int arg1)
{
	textStyle_.backgroundPadding = arg1;

	if(!isSettingUp_) {
		ui->cbBackground->setChecked(true);
		textStyle_.backgroundEnabled = true;

		emit sigTextStyleChanged(textStyle_);
	}
}

void TextStyleWidget::on_wgtBackgroundColor_sigColorChanged(const QColor &color)
{
	textStyle_.backgroundColor = color;

	if(!isSettingUp_) {
		ui->cbBackground->setChecked(true);
		textStyle_.backgroundEnabled = true;

		emit sigTextStyleChanged(textStyle_);
	}
}
