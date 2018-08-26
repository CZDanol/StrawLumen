#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGlobal>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>

#include "wgt/displayselectionwidget.h"
#include "wgt/styleselectionwidget.h"
#include "rec/presentationstyle.h"
#include "gui/settingsdialog.h"
#include "job/widgetvalues.h"

#define UI_SETTINGSDIALOGPATH "ui_settingsdialog.h"
#include UI_SETTINGSDIALOGPATH

class SettingsManager {

public:
	SettingsManager();
	~SettingsManager();

public:
	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &def = QVariant()) const;

public:
	QRect projectionDisplayGeometry() const;

private:
	QSettings settings_;

public:
#define F(settingName, uiControl)\
	inline decltype(auto) setting_ ## settingName() const {\
		return getWidgetValue(settingsDialog->ui->uiControl);\
	}

	SETTINGS_FACTORY(F)
#undef F

};

extern SettingsManager *settings;

template<typename T>
inline void saveSetting(const QString &name, const T *widget) { Q_UNUSED(name); Q_UNUSED(widget); throw 0; }

template<typename T>
inline void loadSetting(const QString &name, T *widget) { Q_UNUSED(name); Q_UNUSED(widget); throw 0; }

template<typename T>
inline auto settingsControlChangeSignal(T *widget) { Q_UNUSED(widget); throw 0; }

#define SETTING_HEADERS(T, signal)\
	template<> void saveSetting<T>(const QString &name, const T *widget);\
	template<> void loadSetting<T>(const QString &name, T *widget);\
	template<> inline auto settingsControlChangeSignal<T>(T*) {return signal;}


SETTING_HEADERS(QComboBox, QOverload<int>::of(&QComboBox::activated))
SETTING_HEADERS(QSpinBox, &QSpinBox::editingFinished)
SETTING_HEADERS(QCheckBox, &QCheckBox::clicked)
SETTING_HEADERS(QGroupBox, &QGroupBox::clicked)

SETTING_HEADERS(DisplaySelectionWidget, &DisplaySelectionWidget::sigCurrentChangedByUser)
SETTING_HEADERS(StyleSelectionWidget, &StyleSelectionWidget::sigPresentationStyleChangedByUser)

#undef SETTING_HEADERS

#endif // SETTINGS_H
