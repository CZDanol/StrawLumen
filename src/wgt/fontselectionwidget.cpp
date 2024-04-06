#include "fontselectionwidget.h"
#include "ui_fontselectionwidget.h"

#include "job/fontdatabase.h"

FontSelectionWidget::FontSelectionWidget(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::FontSelectionWidget) {
	ui->setupUi(this);
	ui->cmbFont->setModel(&fontListModel_);
	ui->cmbFontStyle->setModel(&fontStyleListModel_);

	updateFontList();
}

FontSelectionWidget::~FontSelectionWidget() {
	delete ui;
}

const QFont &FontSelectionWidget::selectedFont() const {
	return selectedFont_;
}

void FontSelectionWidget::updateFontList() {
	fontListModel_.setStringList(fontDatabase().families());
}

void FontSelectionWidget::setSelectedFont(const QFont &font) {
	if(font == selectedFont_)
		return;

	selectedFont_ = font;

	ui->cmbFont->setCurrentIndex(fontListModel_.stringList().indexOf(font.family()));
	ui->sbSize->setValue(font.pointSize());
	updateFontStyleList(font.styleName());

	ui->btnBold->setChecked(font.bold());
	ui->btnItalic->setChecked(font.italic());
}

void FontSelectionWidget::setReadOnly(bool set) {
	if(isReadOnly_ == set)
		return;

	isReadOnly_ = set;
	ui->cmbFont->setEnabled(!set);
	ui->cmbFontStyle->setEnabled(!set);
	ui->sbSize->setReadOnly(set);
	ui->btnBold->setEnabled(!set);
	ui->btnItalic->setEnabled(!set);
}

void FontSelectionWidget::updateFontStyleList(const QString &setStyle) {
	fontStyleListModel_.setStringList(fontDatabase().styles(selectedFont_.family()));

	const QStringList stringList = fontStyleListModel_.stringList();
	int pos = stringList.indexOf(setStyle);
	if(pos == -1)
		pos = 0;

	ui->cmbFontStyle->setCurrentIndex(pos);

	if(stringList.size())
		on_cmbFontStyle_activated(pos);
}

void FontSelectionWidget::on_cmbFont_activated(int) {
	const QString arg1 = ui->cmbFont->currentText();

	if(selectedFont_.family() == arg1)
		return;

	const QString fontStyleName = selectedFont_.styleName();
	selectedFont_.setFamily(arg1);
	updateFontStyleList(fontStyleName);

	emit sigFontChangedByUser(selectedFont_);
}

void FontSelectionWidget::on_cmbFontStyle_activated(int) {
	const QString arg1 = ui->cmbFontStyle->currentText();

	if(selectedFont_.styleName() == arg1)
		return;

	selectedFont_ = fontDatabase().font(ui->cmbFont->currentText(), ui->cmbFontStyle->currentText(), ui->sbSize->value());
	ui->btnBold->setChecked(selectedFont_.bold());
	ui->btnItalic->setChecked(selectedFont_.italic());

	emit sigFontChangedByUser(selectedFont_);
}

void FontSelectionWidget::on_sbSize_valueChanged(int arg1) {
	if(selectedFont_.pointSize() == arg1)
		return;

	selectedFont_.setPointSize(arg1);

	emit sigFontChangedByUser(selectedFont_);
}

void FontSelectionWidget::on_btnBold_toggled(bool checked) {
	if(selectedFont_.bold() == checked)
		return;

	selectedFont_.setBold(checked);
	emit sigFontChangedByUser(selectedFont_);
}

void FontSelectionWidget::on_btnItalic_toggled(bool checked) {
	if(selectedFont_.italic() == checked)
		return;

	selectedFont_.setItalic(checked);
	emit sigFontChangedByUser(selectedFont_);
}
