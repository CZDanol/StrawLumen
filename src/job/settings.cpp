#include "settings.h"

#include <QDir>

#include "gui/settingsdialog.h"
#include "util/standarddialogs.h"
#include "main.h"

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

#define SAVE_SETTING(Type) template<> void saveSetting<Type>(const QString &name, Type *control)
#define LOAD_SETTING(Type) template<> void loadSetting<Type>(const QString &name, Type *control)

SAVE_SETTING(DisplaySelectionWidget)
{
	auto id = control->selectedScreenId();
	settings->setValue(name + ".geometry", id.first);
	settings->setValue(name + ".name", id.second);
}

LOAD_SETTING(DisplaySelectionWidget)
{
	QPair<QRect, QString> id(
				settings->value(name + ".geometry").toRect(),
				settings->value(name + ".name").toString()
				);
	control->setSelectedScreen(id);
}
