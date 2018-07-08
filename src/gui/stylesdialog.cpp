#include "stylesdialog.h"
#include "ui_stylesdialog.h"

#include <QSqlRecord>

#include "util/standarddialogs.h"
#include "job/db.h"

//F(settingsName, uiControl)
#define STYLE_FIELDS_FACTORY(F) \
	F("name", lnName) \
	F("mainTextStyle", wgtMainTextStyle)
	//F("background", btn)

extern StylesDialog *stylesDialog = nullptr;

StylesDialog::StylesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StylesDialog)
{
	ui->setupUi(this);
	ui->twGallery->setCornerWidget(ui->twGalleryCorner);
	ui->twProperties->setCornerWidget(ui->twPropertiesCorner);

	ui->lvList->setModel(&styleListModel_);

	connect(&presentationStyle_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
	connect(ui->lvList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentStyleChanged(QModelIndex,QModelIndex)));

	isEditMode_ = true;
	setEditMode(false);
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

void StylesDialog::setEditMode(bool set)
{
	ui->btnSaveChanges->setVisible(set);
	ui->btnDiscardChanges->setVisible(set);
	ui->btnEdit->setVisible(!set);

	ui->btnAdd->setEnabled(!set);

#define F(settingsName, uiControl) \
	ui->uiControl->setReadOnly(!set);\
	ui->uiControl->style()->unpolish(ui->uiControl); \
	ui->uiControl->style()->polish(ui->uiControl);

	STYLE_FIELDS_FACTORY(F)
		#undef F
}

bool StylesDialog::tryCloseEditMode()
{
	if(!isEditMode_)
		return true;

	if(!standardDeleteConfirmDialog(tr("Aktuální styl je otevřený pro editaci. Chcete pokračovat a zahodit provedené úpravy?")))
		return false;

	setEditMode(false);
	return true;
}

void StylesDialog::onStyleChanged()
{
	ui->wgtPreview->setPresentationStyle(presentationStyle_);
}

void StylesDialog::onCurrentStyleChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex)
{
	if(!newIndex.isValid())
		return;

	const qlonglong newId = styleListModel_.record(newIndex.row()).value("id").toLongLong();
	if(newId == currentStyleId_)
		return;

	if(!tryCloseEditMode()) {
		ui->lvList->setCurrentIndex(oldIndex);
		return;
	}

	currentStyleId_ = newId;
	fillStyleData();
}

void StylesDialog::fillStyleData()
{
	QSqlRecord rec = db->selectRow("SELECT * FROM styles WHERE id = ?", {currentStyleId_});
}

void StylesDialog::on_widget_sigTextStyleChangedByUser(const TextStyle &style)
{
	presentationStyle_.setMainTextStyle(style);
}

void StylesDialog::on_btnAdd_clicked()
{
	currentStyleId_ = -1;
	setEditMode(true);
}

void StylesDialog::on_btnDiscardChanges_clicked()
{
	if(!standardConfirmDialog(tr("Opravdu zahodit provedené úpravy?")))
		return;

	setEditMode(false);
	fillStyleData(); // Reload original values
}
