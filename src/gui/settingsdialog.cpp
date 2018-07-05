#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "job/settings.h"
#include "presentation/presentationengine.h"
#include "presentation/presentationmanager.h"
#include "util/chord.h"

#define SETTINGS_FACTORY(F) \
	F(display, dsDisplay)

SettingsDialog *settingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->dsDisplay, SIGNAL(sigCurrentChanged(QScreen*)), this, SLOT(onDisplayChanged(QScreen*)));

#define F(name, control)\
		loadSetting(#name, ui->control);\
		connect(ui->control, settingsControlChangeSignal(ui->control), [this]{\
			saveSetting(#name, ui->control);\
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
