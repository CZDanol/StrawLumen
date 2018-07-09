#include "settings.h"

#include <QDir>

#include "gui/settingsdialog.h"
#include "util/standarddialogs.h"
#include "main.h"

#define UI_SETTINGSDIALOGPATH "ui_settingsdialog.h"
#include UI_SETTINGSDIALOGPATH

#define SETTING_SAVE(T) template<> void saveSetting<T>(const QString &name, const T *widget)
#define SETTING_LOAD(T) template<> void loadSetting<T>(const QString &name, T *widget)

SettingsManager *settings = nullptr;

SettingsManager::SettingsManager() :
	settings_(appDataDirectory.absoluteFilePath("settings.ini"), QSettings::IniFormat)
{
	if(!settings_.isWritable())
		standardErrorDialog(SettingsDialog::tr("Nepodařilo se otevřít nastavení programu pro zápis. Nastavení se nebude ukládat."));
}

SettingsManager::~SettingsManager()
{

}

void SettingsManager::setValue(const QString &key, const QVariant &value)
{
	settings_.setValue(key, value);
}

QVariant SettingsManager::value(const QString &key, const QVariant &def) const
{
	return settings_.value(key, def);
}

QRect SettingsManager::projectionDisplayGeometry() const
{
	return settingsDialog->ui->dsDisplay->selectedScreen()->geometry();
}

const PresentationStyle &SettingsManager::defaultPresentationStyle() const
{
	return settingsDialog->ui->wgtDefaultPresentationStyle->presentationStyle();
}


SETTING_SAVE(DisplaySelectionWidget)
{
	auto id = widget->selectedScreenId();
	settings->setValue(name + ".geometry", id.first);
	settings->setValue(name + ".name", id.second);
}

SETTING_LOAD(DisplaySelectionWidget)
{
	QPair<QRect, QString> id(
				settings->value(name + ".geometry").toRect(),
				settings->value(name + ".name").toString()
				);
	widget->setSelectedScreen(id);
}
