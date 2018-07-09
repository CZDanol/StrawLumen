#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "job/settings.h"
#include "job/db.h"
#include "presentation/presentationengine.h"
#include "presentation/presentationmanager.h"

// F(settingsName, uiControl)
#define SETTINGS_FACTORY(F) \
	F("display", dsDisplay)

SettingsDialog *settingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->dsDisplay, SIGNAL(sigCurrentChangedByUser(QScreen*)), this, SLOT(onDisplayChanged(QScreen*)));

#define F(settingsName, uiControl)\
		loadSetting(settingsName, ui->uiControl);\
		connect(ui->uiControl, settingsControlChangeSignal(ui->uiControl), [this]{\
			saveSetting(settingsName, ui->uiControl);\
		});

	SETTINGS_FACTORY(F)
#undef F

	fillData();
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::fillData()
{
	PresentationStyle style;
	style.loadFromDb(settings->value("song.defaultStyleId", 1).toLongLong());
	ui->wgtDefaultPresentationStyle->setPresentationStyle(style);

	ui->cbEmptySlideBeforeSong->setChecked(settings->value("song.emptySlideBefore", false).toBool());
	ui->cbEmptySlideAfterSong->setChecked(settings->value("song.emptySlideAfter", true).toBool());
}

void SettingsDialog::onDisplayChanged(QScreen *current)
{
	if(presentationManager->isActive())
		presentationManager->currentEngine()->setDisplay(current->geometry());
}

void SettingsDialog::on_btnClose_clicked()
{
	accept();
}

void SettingsDialog::on_wgtDefaultPresentationStyle_sigPresentationStyleChangedByUser()
{
	settings->setValue("song.defaultStyleId", ui->wgtDefaultPresentationStyle->presentationStyle().styleId());
}

void SettingsDialog::on_cbEmptySlideBeforeSong_clicked(bool checked)
{
	settings->setValue("song.emptySlideBefore", checked);
}

void SettingsDialog::on_cbEmptySlideAfterSong_clicked(bool checked)
{
	settings->setValue("song.emptySlideAfter", checked);
}
