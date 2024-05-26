#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QSettings>
#include <QSpinBox>
#include <QtGlobal>

#include "gui/settingsdialog.h"
#include "job/widgetvalues.h"
#include "rec/presentationstyle.h"
#include "wgt/bibletranslationlistbox.h"
#include "wgt/displayselectionwidget.h"
#include "wgt/styleselectionwidget.h"

#define UI_SETTINGSDIALOGPATH "ui_settingsdialog.h"
#include UI_SETTINGSDIALOGPATH

extern const QString bibleTranslationListSep;

#define SETTINGS_LINK(settingsName, uiControl)                                \
	loadSetting(#settingsName, ui->uiControl);                                  \
	connect(ui->uiControl, settingsControlChangeSignal(ui->uiControl), [this] { \
		saveSetting(#settingsName, ui->uiControl);                                \
	});

class SettingsManager {

public:
	SettingsManager();
	~SettingsManager();

public:
	void setValue(const QString &key, const QVariant &value);
	QVariant value(const QString &key, const QVariant &def = QVariant()) const;

public:
	QScreen *projectionDisplay() const;
	QRect projectionDisplayGeometry() const;

private:
	QSettings settings_;

public:
#define F(settingName, uiControl)                         \
	inline decltype(auto) setting_##settingName() const {   \
		return getWidgetValue(settingsDialog->ui->uiControl); \
	}

	SETTINGS_FACTORY(F)
#undef F
};

extern SettingsManager *settings;

template<typename T>
inline void saveSetting(const QString &name, const T *widget) {
	Q_UNUSED(name)
	Q_UNUSED(widget)
	throw 0;
}

template<typename T>
inline void loadSetting(const QString &name, T *widget) {
	Q_UNUSED(name)
	Q_UNUSED(widget)
	throw 0;
}

template<typename T>
inline auto settingsControlChangeSignal(T *widget) {
	Q_UNUSED(widget)
	throw 0;
}

#define SETTING_HEADERS(T, signal)                           \
	template<>                                                 \
	void saveSetting<T>(const QString &name, const T *widget); \
	template<>                                                 \
	void loadSetting<T>(const QString &name, T *widget);       \
	template<>                                                 \
	inline auto settingsControlChangeSignal<T>(T *) { return signal; }

SETTING_HEADERS(QComboBox, QOverload<int>::of(&QComboBox::activated))
SETTING_HEADERS(QSpinBox, &QSpinBox::editingFinished)
SETTING_HEADERS(QCheckBox, &QCheckBox::clicked)
SETTING_HEADERS(QGroupBox, &QGroupBox::clicked)

SETTING_HEADERS(DisplaySelectionWidget, &DisplaySelectionWidget::sigCurrentChangedByUser)
SETTING_HEADERS(StyleSelectionWidget, &StyleSelectionWidget::sigPresentationStyleChangedByUser)
SETTING_HEADERS(BibleTranslationListBox, &BibleTranslationListBox::sigChanged)

#undef SETTING_HEADERS

#endif// SETTINGS_H
