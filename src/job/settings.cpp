#include "settings.h"

#include <QDir>

#include "gui/settingsdialog.h"
#include "util/standarddialogs.h"
#include "main.h"

#define SETTING_SAVE(Type) template<> void saveSetting<Type>(const QString &name, const Type *widget)
#define SETTING_LOAD(Type) template<> void loadSetting<Type>(const QString &name, Type *widget)

QSettings *settings = nullptr;

void initSettings()
{
	settings = new QSettings(appDataDirectory.absoluteFilePath("settings.ini"), QSettings::IniFormat);

	if(!settings->isWritable())
		standardErrorDialog(SettingsDialog::tr("Nepodařilo se otevřít nastavení programu pro zápis. Nastavení se nebude ukládat."));
}

void uninitSettings()
{
	delete settings;
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
