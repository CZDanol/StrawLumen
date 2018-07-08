#include "textstylewidget.h"
#include "ui_textstylewidget.h"

TextStyleWidget::TextStyleWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TextStyleWidget)
{
	ui->setupUi(this);

	connect(this, SIGNAL(sigTextStyleChangedByUser(TextStyle)), ui->wgtPreview, SLOT(setTextStyle(TextStyle)));

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

	ui->cbBackground->setChecked(style.backgroundEnabled);
	ui->wgtBackgroundColor->setColor(style.backgroundColor);
	ui->sbBackgroundPadding->setValue(style.backgroundPadding);

	ui->wgtPreview->forceSetTextStyle(textStyle_);

	isSettingUp_ = false;
}

void TextStyleWidget::setReadOnly(bool set)
{
	if(isReadOnly_ == set)
		return;

	isReadOnly_ = set;
	ui->wgtBackgroundColor->setReadOnly(set);
	ui->wgtColor->setReadOnly(set);
	ui->wgtFont->setReadOnly(set);
	ui->wgtOutlineColor->setReadOnly(set);

	ui->sbBackgroundPadding->setReadOnly(set);
	ui->sbOutlineWidth->setReadOnly(set);

	ui->cbBackground->setEnabled(!set);
	ui->cbOutline->setEnabled(!set);
}

void TextStyleWidget::on_wgtFont_sigFontChangedByUser(const QFont &font)
{
	textStyle_.font = font;
	emit sigTextStyleChangedByUser(textStyle_);
}

void TextStyleWidget::on_wgtColor_sigColorChangedByUser(const QColor &color)
{
	textStyle_.color = color;
	emit sigTextStyleChangedByUser(textStyle_);
}

void TextStyleWidget::on_cbOutline_clicked(bool checked)
{
	textStyle_.outlineEnabled = checked;
	emit sigTextStyleChangedByUser(textStyle_);
}

void TextStyleWidget::on_sbOutlineWidth_valueChanged(int arg1)
{
	textStyle_.outlineWidth = arg1;

	if(!isSettingUp_) {
		ui->cbOutline->setChecked(true);
		textStyle_.outlineEnabled = true;

		emit sigTextStyleChangedByUser(textStyle_);
	}
}

void TextStyleWidget::on_wgtOutlineColor_sigColorChangedByUser(const QColor &color)
{
	textStyle_.outlineColor = color;

	if(!isSettingUp_) {
		ui->cbOutline->setChecked(true);
		textStyle_.outlineEnabled = true;

		emit sigTextStyleChangedByUser(textStyle_);
	}
}

void TextStyleWidget::on_cbBackground_clicked(bool checked)
{
	textStyle_.backgroundEnabled = checked;
	emit sigTextStyleChangedByUser(textStyle_);
}

void TextStyleWidget::on_sbBackgroundPadding_valueChanged(int arg1)
{
	textStyle_.backgroundPadding = arg1;

	if(!isSettingUp_) {
		ui->cbBackground->setChecked(true);
		textStyle_.backgroundEnabled = true;

		emit sigTextStyleChangedByUser(textStyle_);
	}
}

void TextStyleWidget::on_wgtBackgroundColor_sigColorChangedByUser(const QColor &color)
{
	textStyle_.backgroundColor = color;

	if(!isSettingUp_) {
		ui->cbBackground->setChecked(true);
		textStyle_.backgroundEnabled = true;

		emit sigTextStyleChangedByUser(textStyle_);
	}
}
