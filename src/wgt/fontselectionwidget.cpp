#include "fontselectionwidget.h"
#include "ui_fontselectionwidget.h"

#include <QFontDatabase>

FontSelectionWidget::FontSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FontSelectionWidget)
{
	ui->setupUi(this);
	ui->cmbFont->setModel(&fontListModel_);
	ui->cmbFontStyle->setModel(&fontStyleListModel_);

	updateFontList();
}

FontSelectionWidget::~FontSelectionWidget()
{
	delete ui;
}

const QFont &FontSelectionWidget::selectedFont() const
{
	return selectedFont_;
}

void FontSelectionWidget::updateFontList()
{
	fontListModel_.setStringList(fontDatabase_.families());
}

void FontSelectionWidget::setSelectedFont(const QFont &font)
{
	if(font == selectedFont_)
		return;

	selectedFont_ = font;

	ui->cmbFont->setCurrentText(font.family());
	ui->sbSize->setValue(font.pointSize());
	updateFontStyleList(font.styleName());
}

void FontSelectionWidget::updateFontStyleList(const QString &setStyle)
{
	fontStyleListModel_.setStringList(fontDatabase_.styles(selectedFont_.family()));

	const QStringList stringList = fontStyleListModel_.stringList();
	int pos = stringList.indexOf(setStyle);
	if(pos == -1)
		pos = 0;

	ui->cmbFontStyle->setCurrentIndex(pos);

	if(stringList.size())
		selectedFont_.setStyleName(stringList[pos]);
}

void FontSelectionWidget::on_cmbFont_activated(const QString &arg1)
{
	if(selectedFont_.family() == arg1)
		return;

	const QString fontStyleName = selectedFont_.styleName();
	selectedFont_.setFamily(arg1);
	updateFontStyleList(fontStyleName);

	emit sigFontChanged(selectedFont_);
}

void FontSelectionWidget::on_cmbFontStyle_activated(const QString &arg1)
{
	if(selectedFont_.styleName() == arg1)
		return;

	selectedFont_.setStyleName(arg1);

	emit sigFontChanged(selectedFont_);
}

void FontSelectionWidget::on_sbSize_valueChanged(int arg1)
{
	if(selectedFont_.pointSize() == arg1)
		return;

	selectedFont_.setPointSize(arg1);

	emit sigFontChanged(selectedFont_);
}
