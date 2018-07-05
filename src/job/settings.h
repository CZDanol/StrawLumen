#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "gui/displayselectionwidget.h"

extern QSettings *settings;

void initSettings();
void uninitSettings();

template<typename T>
inline void saveSetting(const QString&, T*) { ERROR; }

template<typename T>
inline void loadSetting(const QString&, T*) { ERROR; }

template<typename T>
inline auto settingsControlChangeSignal(T*) { ERROR; }

#define SETTING_HEADERS(Type, signal)\
	template<>\
	void saveSetting<Type>(const QString &name, Type *control);\
	\
	template<>\
	void loadSetting<Type>(const QString &name, Type *control);\
	\
	template<>\
	inline auto settingsControlChangeSignal<Type>(Type*) {\
		return signal;\
	}


SETTING_HEADERS(DisplaySelectionWidget, &DisplaySelectionWidget::sigCurrentChanged)

#undef SETTING_HEADERS

#endif // SETTINGS_H
