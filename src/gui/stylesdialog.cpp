#include "stylesdialog.h"
#include "ui_stylesdialog.h"

#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonObject>

#include "util/standarddialogs.h"
#include "job/db.h"

//F( uiControl)
#define STYLE_FIELDS_FACTORY(F) \
	F(lnName) \
	F(wgtMainTextStyle)\
	F(wgtTitleTextStyle)\
	F(wgtBackground)\
	F(sbTopPadding) F(sbBottomPadding) F(sbLeftPadding) F(sbRightPadding) F(sbTitleTextPadding)

extern StylesDialog *stylesDialog = nullptr;

StylesDialog::StylesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StylesDialog)
{
	ui->setupUi(this);
	ui->twGallery->setCornerWidget(ui->twGalleryCorner);
	ui->twProperties->setCornerWidget(ui->twPropertiesCorner);

	ui->lvList->setModel(&styleListModel_);

	{
		connect(db, SIGNAL(sigStyleListChanged()), this, SLOT(requeryList()));

		connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(accept()));
		connect(ui->btnSelect, SIGNAL(clicked(bool)), this, SLOT(accept()));
		connect(ui->btnStorno, SIGNAL(clicked(bool)), this, SLOT(reject()));
		connect(&presentationStyle_, SIGNAL(sigChanged()), this, SLOT(onStyleChanged()));
		connect(ui->lvList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentStyleChanged(QModelIndex,QModelIndex)));

		connect(ui->wgtMainTextStyle, &TextStyleWidget::sigTextStyleChangedByUser, &presentationStyle_, &PresentationStyle::setMainTextStyle);
		connect(ui->wgtTitleTextStyle, &TextStyleWidget::sigTextStyleChangedByUser, &presentationStyle_, &PresentationStyle::setTitleTextStyle);
		connect(ui->lnName, &QLineEdit::editingFinished, this, &StylesDialog::onNameChanged);
		connect(ui->wgtBackground, &PresentationBackgroundSelectionWidget::sigPresentationBackgroundChangedByUser, &presentationStyle_, &PresentationStyle::setBackground);

		using PSMethod = void (PresentationStyle::*)(const int &);
		const auto connectSb = [this](QSpinBox *spinBox, PSMethod method) {
			connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [this, method](int newValue){
				if(!isEditMode_)
					return;

				(presentationStyle_.*method)(newValue);
			});
		};

		connectSb(ui->sbTopPadding, &PresentationStyle::setTopPadding);
		connectSb(ui->sbBottomPadding, &PresentationStyle::setBottomPadding);
		connectSb(ui->sbLeftPadding, &PresentationStyle::setLeftPadding);
		connectSb(ui->sbRightPadding, &PresentationStyle::setRightPadding);
		connectSb(ui->sbTitleTextPadding, &PresentationStyle::setTitleTextPadding);
	}

	{
		listContextMenu_ = new QMenu(this);
		listContextMenu_->addAction(ui->actionDeleteStyle);
	}

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

	if(!ui->lvList->currentIndex().isValid())
		ui->lvList->setCurrentIndex(styleListModel_.index(0,0));

	onCurrentStyleChanged(ui->lvList->currentIndex(), QModelIndex());
}

bool StylesDialog::showInSelectionMode(PresentationStyle &style)
{
	presentationStyle_ = style;

	int row = db->selectValue("SELECT COUNT(*) FROM styles WHERE (name < ?) OR ((name = ?) AND (id < ?))", {style.name(), style.name(), style.styleId()}).toInt();
	requeryList();
	ui->lvList->setCurrentIndex(styleListModel_.index(row,0));

	setMgmtMode(false);

	const bool accepted = QDialog::exec() == QDialog::Accepted;

	if(accepted)
		style = presentationStyle_;

	return accepted;
}

void StylesDialog::showEvent(QShowEvent *e)
{
	requeryList();
	QDialog::showEvent(e);
}

void StylesDialog::reject()
{
	if(!askFinishEditMode())
		return;

	QDialog::reject();
}

void StylesDialog::accept()
{
	if(!askFinishEditMode())
		return;

	QDialog::accept();
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
	isEditMode_ = set;

	ui->btnSaveChanges->setVisible(set);
	ui->btnDiscardChanges->setVisible(set);
	ui->btnEdit->setVisible(!set);

	ui->btnAdd->setEnabled(!set);

#define F(uiControl) \
	ui->uiControl->setReadOnly(!set);\
	ui->uiControl->style()->unpolish(ui->uiControl); \
	ui->uiControl->style()->polish(ui->uiControl);

	STYLE_FIELDS_FACTORY(F)
		#undef F

	updateManipulationButtonsEnabled();
}

bool StylesDialog::askFinishEditMode()
{
	if(!isEditMode_)
		return true;

	if(standardConfirmDialog(tr("Aktuální styl je otevřený pro editaci. Chcete uložit provedené úpravy?")))
		ui->btnSaveChanges->click();
	else {
		setEditMode(false);
		fillStyleData();
	}

	// TODO: maybe add close button?
	return true;
}

void StylesDialog::fillStyleData()
{
	if(currentStyleId_ == -1)
		return;

	currentStyleIsInternal_ = db->selectValue("SELECT isInternal FROM styles WHERE id = ?", {currentStyleId_}).toBool();
	ui->btnEdit->setEnabled(!currentStyleIsInternal_);

	presentationStyle_.loadFromDb(currentStyleId_);

	ui->lnName->setText(presentationStyle_.name());
	ui->wgtMainTextStyle->setTextStyle(presentationStyle_.mainTextStyle());
	ui->wgtTitleTextStyle->setTextStyle(presentationStyle_.titleTextStyle());
	ui->wgtBackground->setPresentationBackground(presentationStyle_.background());

	ui->sbTopPadding->setValue(presentationStyle_.topPadding());
	ui->sbBottomPadding->setValue(presentationStyle_.bottomPadding());
	ui->sbLeftPadding->setValue(presentationStyle_.leftPadding());
	ui->sbRightPadding->setValue(presentationStyle_.rightPadding());
	ui->sbTitleTextPadding->setValue(presentationStyle_.titleTextPadding());
}

void StylesDialog::requeryList()
{
	const int rowId = ui->lvList->currentIndex().row();
	const qlonglong id = currentStyleId_;

	styleListModel_.setQuery(db->selectQuery(QString("SELECT (CASE WHEN isInternal THEN (name || ?) ELSE name END) AS displayName, id FROM styles ORDER BY name ASC, id ASC"), {tr(" (interní)")}));

	if(rowId != -1 && styleListModel_.record(rowId).value("id").toLongLong() == id)
		ui->lvList->setCurrentIndex(styleListModel_.index(rowId, 0));
}

void StylesDialog::updateManipulationButtonsEnabled()
{
	const bool enabled = currentStyleId_ != -1 && !currentStyleIsInternal_ && !isEditMode_;

	ui->wgtInternalStyleWarning->setVisible(currentStyleIsInternal_);
	ui->btnEdit->setEnabled(enabled);
	ui->actionDeleteStyle->setEnabled(enabled);
}

void StylesDialog::onStyleChanged()
{
	ui->wgtPreview->setPresentationStyle(presentationStyle_);
}

void StylesDialog::onCurrentStyleChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex)
{
	const qlonglong newId = styleListModel_.record(newIndex.row()).value("id").toLongLong();
	if(newId == currentStyleId_)
		return;

	if(!askFinishEditMode()) {
		ui->lvList->setCurrentIndex(oldIndex);
		return;
	}

	currentStyleId_ = newId;
	fillStyleData();
	updateManipulationButtonsEnabled();
}

void StylesDialog::onNameChanged()
{
	presentationStyle_.setName(ui->lnName->text());
}

void StylesDialog::on_btnAdd_clicked()
{
	currentStyleId_ = -1;
	currentStyleIsInternal_ = false;
	ui->lnName->setText(tr("Nový styl"));

	setEditMode(true);
	ui->twProperties->setCurrentWidget(ui->tabGeneral);
	ui->lnName->setFocus();
	ui->lnName->selectAll();
}

void StylesDialog::on_btnDiscardChanges_clicked()
{
	if(!standardConfirmDialog(tr("Opravdu zahodit provedené úpravy?")))
		return;

	setEditMode(false);
	fillStyleData(); // Reload original values
}

void StylesDialog::on_btnSaveChanges_clicked()
{
	if(currentStyleIsInternal_)
		return;

	QHash<QString,QVariant> data{
		{"name", presentationStyle_.name()},
		{"data", QJsonDocument(presentationStyle_.toJSON()).toJson(QJsonDocument::Compact)}
	};

	if(currentStyleId_ == -1) {
		data.insert("isInternal", false);
		currentStyleId_ = db->insert("styles", data).toLongLong();

	} else
		db->update("styles", data, "id = ?", {currentStyleId_});

	setEditMode(false);
	emit db->sigStyleChanged(currentStyleId_);

	presentationStyle_.assumeStyleId(currentStyleId_);
}

void StylesDialog::on_btnEdit_clicked()
{
	setEditMode(true);
}

void StylesDialog::on_lvList_customContextMenuRequested(const QPoint &pos)
{
	if(currentStyleId_ == -1)
		return;

	listContextMenu_->popup(ui->lvList->viewport()->mapToGlobal(pos));
}

void StylesDialog::on_actionDeleteStyle_triggered()
{
	if(currentStyleId_ == -1)
		return;

	if(isEditMode_)
		return standardErrorDialog(tr("V režimu úprav nelze mazat styly. Ukončete úpravy a zkuste to znovu."));

	if(!standardDeleteConfirmDialog(tr("Opravdu smazat styl \"%1\"?").arg(ui->lnName->text())))
		return;

	db->exec("DELETE FROM styles WHERE id = ?", {currentStyleId_});

	emit db->sigStyleChanged(currentStyleId_);
	currentStyleId_ = -1;
	updateManipulationButtonsEnabled();
}

void StylesDialog::on_lvList_activated(const QModelIndex &)
{
	if(isMgmtMode_)
		ui->btnEdit->click();
	else
		accept();
}
