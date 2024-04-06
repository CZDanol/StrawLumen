#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QJsonDocument>
#include <QJsonObject>

#include "job/db.h"
#include "job/settings.h"
#include "main.h"
#include "presentation/presentationengine.h"
#include "presentation/presentationmanager.h"
#include "util/updatesdisabler.h"

SettingsDialog *settingsDialog = nullptr;

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent),
                                                  ui(new Ui::SettingsDialog) {
	ui->setupUi(this);

	connect(ui->dsDisplay, SIGNAL(sigCurrentChangedByUser(QScreen *)), this, SLOT(onDisplayChanged(QScreen *)));
	connect(db, &DatabaseManager::sigBibleTranslationsChanged, this, &SettingsDialog::updateBibleTranslationList);

	updateBibleTranslationList();
	updateLanguageList();

	SETTINGS_FACTORY(SETTINGS_LINK)
}

SettingsDialog::~SettingsDialog() {
	delete ui;
}

void SettingsDialog::updateBibleTranslationList() {
	ui->lstDefaultBibleTranslation->updateList();
}

void SettingsDialog::updateLanguageList() {
	ui->cmbLanguage->clear();
	ui->cmbLanguage->addItem("Čeština", "cz");

	QDir dir(QDir(qApp->applicationDirPath()).absoluteFilePath("../lang"));
	for(const QFileInfo &f: dir.entryInfoList({"*.qm"}))
		ui->cmbLanguage->addItem(QLocale::languageToString(QLocale(f.baseName()).language()), f.baseName());
}

void SettingsDialog::onDisplayChanged(QScreen *current) {
	if(presentationManager->isActive())
		presentationManager->currentEngine()->setDisplay(current->geometry());
}

void SettingsDialog::on_btnClose_clicked() {
	accept();
}

void SettingsDialog::on_cbDarkMode_clicked(bool checked) {
	setupStylesheet(checked);
}
