#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "wgt/displayselectionwidget.h"

extern QSettings *settings;

void initSettings();
void uninitSettings();

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


SETTING_HEADERS(DisplaySelectionWidget, &DisplaySelectionWidget::sigCurrentChangedByUser)

#undef SETTING_HEADERS

#endif // SETTINGS_H
