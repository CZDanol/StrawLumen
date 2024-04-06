#include "styleselectionwidget.h"
#include "ui_styleselectionwidget.h"

#include "gui/stylesdialog.h"

StyleSelectionWidget::StyleSelectionWidget(QWidget *parent) : QWidget(parent),
                                                              ui(new Ui::StyleSelectionWidget) {
	ui->setupUi(this);

	connect(&presentationStyle_, SIGNAL(sigChanged()), this, SLOT(updateLabel()));
}

StyleSelectionWidget::~StyleSelectionWidget() {
	delete ui;
}

const PresentationStyle &StyleSelectionWidget::presentationStyle() const {
	return presentationStyle_;
}

void StyleSelectionWidget::setPresentationStyle(const PresentationStyle &style) {
	if(style == presentationStyle_)
		return;

	presentationStyle_ = style;
	// No need to call updateLabel, PresentationStyle.sigChanged is emitted
}

void StyleSelectionWidget::updateLabel() {
	ui->btnSelect->setText(presentationStyle_.name());
}

void StyleSelectionWidget::on_btnSelect_clicked() {
	if(stylesDialog->showInSelectionMode(presentationStyle_))
		emit sigPresentationStyleChangedByUser();
}
