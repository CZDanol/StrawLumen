#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "main.h"
#include "job/settings.h"
#include "job/db.h"
#include "presentation/presentationengine.h"
#include "presentation/presentationmanager.h"
#include "util/updatesdisabler.h"

SettingsDialog *settingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	connect(ui->dsDisplay, SIGNAL(sigCurrentChangedByUser(QScreen*)), this, SLOT(onDisplayChanged(QScreen*)));
	connect(db, &DatabaseManager::sigBibleTranslationsChanged, this, &SettingsDialog::updateBibleTranslationList);

	ui->cmbDefaultBibleTranslation->addItem(nullptr, "ČEP"); // ČEP default
	updateBibleTranslationList();

#define F(settingsName, uiControl)\
		loadSetting(#settingsName, ui->uiControl);\
		connect(ui->uiControl, settingsControlChangeSignal(ui->uiControl), [this]{\
			saveSetting(#settingsName, ui->uiControl);\
		});

	SETTINGS_FACTORY(F)
#undef F
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::updateBibleTranslationList()
{
	UpdatesDisabler _ud(ui->cmbDefaultBibleTranslation);
	QVariant data = ui->cmbDefaultBibleTranslation->currentData();

	ui->cmbDefaultBibleTranslation->clear();

	QSqlQuery q = db->selectQuery("SELECT translation_id, name FROM bible_translations ORDER BY translation_id");
	while(q.next()) {
		ui->cmbDefaultBibleTranslation->addItem(q.value(0).toString() + " | " + q.value(1).toString(), q.value(0));
		if(q.value(0) == data)
			ui->cmbDefaultBibleTranslation->setCurrentIndex(ui->cmbDefaultBibleTranslation->count() - 1);
	}
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

void SettingsDialog::on_cbDarkMode_clicked(bool checked)
{
	setupStylesheet(checked);
}
