#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "job/settings.h"
#include "job/db.h"
#include "presentation/presentationengine.h"
#include "presentation/presentationmanager.h"
#include "rec/chord.h"

// F(settingsName, uiControl)
#define SETTINGS_FACTORY(F) \
	F("display", dsDisplay)

SettingsDialog *settingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	settings_.reset(new Settings(this));

	connect(ui->dsDisplay, SIGNAL(sigCurrentChangedByUser(QScreen*)), this, SLOT(onDisplayChanged(QScreen*)));

#define F(settingsName, uiControl)\
		loadSetting(settingsName, ui->uiControl);\
		connect(ui->uiControl, settingsControlChangeSignal(ui->uiControl), [this]{\
			saveSetting(settingsName, ui->uiControl);\
		});

	SETTINGS_FACTORY(F)
#undef F

	defaultPresentationStyle_.loadFromDb(db->selectValueDef("SELECT id FROM styles LIMIT 1").toLongLong());
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

const SettingsDialog::Settings &SettingsDialog::settings() const
{
	return *settings_;
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

SettingsDialog::Settings::Settings(SettingsDialog *dlg) : dlg(*dlg)
{

}

QRect SettingsDialog::Settings::projectionDisplayGeometry() const
{
	return dlg.ui->dsDisplay->selectedScreen()->geometry();
}

const PresentationStyle &SettingsDialog::Settings::defaultPresentationStyle() const
{
	return dlg.defaultPresentationStyle_;
}
