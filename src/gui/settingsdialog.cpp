#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "presentation/presentationengine.h"
#include "presentation/presentationmanager.h"

SettingsDialog *settingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->dsDisplay, SIGNAL(sigCurrentChanged(QScreen*)), this, SLOT(onDisplayChanged(QScreen*)));
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

QRect SettingsDialog::projectionDisplayGeometry()
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
