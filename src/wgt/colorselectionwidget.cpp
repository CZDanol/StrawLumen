#include "colorselectionwidget.h"
#include "ui_colorselectionwidget.h"

#include <QColorDialog>

ColorSelectionWidget::ColorSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorSelectionWidget)
{
	ui->setupUi(this);
}

ColorSelectionWidget::~ColorSelectionWidget()
{
	delete ui;
}

void ColorSelectionWidget::on_btnSelect_clicked()
{
	QColorDialog dlg(this);
	dlg.setOption(QColorDialog::ShowAlphaChannel);
	dlg.exec();
}
