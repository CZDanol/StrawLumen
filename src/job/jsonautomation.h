#ifndef JSONAUTOMATION_H
#define JSONAUTOMATION_H

#include <QJsonValue>
#include <QString>
#include <QColor>
#include <QFont>

#include "rec/textstyle.h"
#include "rec/presentationbackground.h"

template<typename T>
inline void loadFromJSON(T &subj, const QJsonValue &json) { Q_UNUSED(subj); Q_UNUSED(json); __ERROR__; }

template<typename T>
inline QJsonValue toJSON(const T &subj) { Q_UNUSED(subj); __ERROR__; }

#define JSON_HEADERS(T) \
	template<> void loadFromJSON<T>(T &subj, const QJsonValue &json);\
	template<> QJsonValue toJSON<T>(const T &subj);

JSON_HEADERS(bool)
JSON_HEADERS(int)

JSON_HEADERS(QString)
JSON_HEADERS(QColor)
JSON_HEADERS(QFont)

JSON_HEADERS(TextStyle)
JSON_HEADERS(PresentationBackground)

#undef JSON_HEADERS

#endif // JSONAUTOMATION_H
