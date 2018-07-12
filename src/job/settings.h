#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGlobal>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>

#include "wgt/displayselectionwidget.h"
#include "rec/presentationstyle.h"

class SettingsManager {

public:
	SettingsManager();
	~SettingsManager();

public:
	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &def = QVariant()) const;

public:
	QRect projectionDisplayGeometry() const;
	const PresentationStyle &defaultPresentationStyle() const;

private:
	QSettings settings_;

};

extern SettingsManager *settings;

template<typename T>
inline void saveSetting(const QString &name, const T *widget) { Q_UNUSED(name); Q_UNUSED(widget); __ERROR__; }

template<typename T>
inline void loadSetting(const QString &name, T *widget) { Q_UNUSED(name); Q_UNUSED(widget); __ERROR__; }

template<typename T>
inline auto settingsControlChangeSignal(T *widget) { Q_UNUSED(widget); __ERROR__; }

#define SETTING_HEADERS(T, signal)\
	template<> void saveSetting<T>(const QString &name, const T *widget);\
	template<> void loadSetting<T>(const QString &name, T *widget);\
	template<> inline auto settingsControlChangeSignal<T>(T*) {return signal;}


SETTING_HEADERS(QComboBox, QOverload<int>::of(&QComboBox::activated))
SETTING_HEADERS(QSpinBox, &QSpinBox::editingFinished)
SETTING_HEADERS(QCheckBox, &QCheckBox::clicked)
SETTING_HEADERS(QGroupBox, &QGroupBox::clicked)

SETTING_HEADERS(DisplaySelectionWidget, &DisplaySelectionWidget::sigCurrentChangedByUser)

#undef SETTING_HEADERS

#endif // SETTINGS_H
