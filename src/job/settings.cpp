#include "settings.h"

#include <QDir>

#include "gui/settingsdialog.h"
#include "util/standarddialogs.h"
#include "main.h"

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

SETTING_SAVE(QComboBox)
{
	settings->setValue(name, widget->currentIndex());
}
SETTING_LOAD(QComboBox)
{
	widget->setCurrentIndex(settings->value(name, widget->currentIndex()).toInt());
}

SETTING_SAVE(QSpinBox)
{
	settings->setValue(name, widget->value());
}
SETTING_LOAD(QSpinBox)
{
	widget->setValue(settings->value(name, widget->value()).toInt());
}

SETTING_SAVE(QCheckBox)
{
	settings->setValue(name, widget->isChecked());
}
SETTING_LOAD(QCheckBox)
{
	widget->setChecked(settings->value(name, widget->isChecked()).toBool());
}

SETTING_SAVE(QGroupBox)
{
	settings->setValue(name, widget->isChecked());
}
SETTING_LOAD(QGroupBox)
{
	widget->setChecked(settings->value(name, widget->isChecked()).toBool());
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

SETTING_SAVE(StyleSelectionWidget)
{
	settings->setValue(name, widget->presentationStyle().styleId());
}
SETTING_LOAD(StyleSelectionWidget)
{
	PresentationStyle style;
	qlonglong id = settings->value(name, 1).toLongLong();
	if(id == -1)
		id = 1;

	style.loadFromDb(id);
	widget->setPresentationStyle(style);
}
