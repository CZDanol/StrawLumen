#ifndef WIDGETVALUES_H
#define WIDGETVALUES_H

#include <QLineEdit>
#include <QCheckBox>

#include "wgt/displayselectionwidget.h"
#include "wgt/styleselectionwidget.h"

template<typename Widget>
inline decltype(auto) getWidgetValue(const Widget *wgt) { Q_UNUSED(wgt); __ERROR__; }

#define WIDGET_VALUE_GETTER(Widget, getterFunction) \
	template<> inline decltype(auto) getWidgetValue<Widget>(const Widget *wgt) {return wgt->getterFunction();}

WIDGET_VALUE_GETTER(QLineEdit, text)
WIDGET_VALUE_GETTER(QCheckBox, isChecked)

WIDGET_VALUE_GETTER(DisplaySelectionWidget, selectedScreen)
WIDGET_VALUE_GETTER(StyleSelectionWidget, presentationStyle)

#endif // WIDGETVALUES_H
