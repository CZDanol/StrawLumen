#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "job/settings.h"
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

	connect(ui->dsDisplay, SIGNAL(sigCurrentChangedByUser(QScreen*)), this, SLOT(onDisplayChanged(QScreen*)));

#define F(settingsName, uiControl)\
		loadSetting(settingsName, ui->uiControl);\
		connect(ui->uiControl, settingsControlChangeSignal(ui->uiControl), [this]{\
			saveSetting(settingsName, ui->uiControl);\
		});

	SETTINGS_FACTORY(F)
#undef F
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

QRect SettingsDialog::projectionDisplayGeometry() const
{
	return ui->dsDisplay->selectedScreen()->geometry();
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
