#include "stylesdialog.h"
#include "ui_stylesdialog.h"

extern StylesDialog *stylesDialog = nullptr;

StylesDialog::StylesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StylesDialog)
{
	ui->setupUi(this);
	ui->twGallery->setCornerWidget(ui->twGalleryCorner);
	ui->twProperties->setCornerWidget(ui->twPropertiesCorner);

	connect(&presentationStyle_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
}

StylesDialog::~StylesDialog()
{
	delete ui;
}

void StylesDialog::showInMgmtMode()
{
	setMgmtMode(true);
	QDialog::show();
}

void StylesDialog::setMgmtMode(bool set)
{
	ui->btnStorno->setVisible(!set);
	ui->btnSelect->setVisible(!set);
	ui->btnClose->setVisible(set);

	isMgmtMode_ = set;
}

void StylesDialog::onStyleChanged()
{

}

void StylesDialog::on_widget_sigTextStyleChangedByUser(const TextStyle &style)
{
	presentationStyle_.setMainTextStyle(style);
}
