#ifndef JSONAUTOMATION_H
#define JSONAUTOMATION_H

#include <QColor>
#include <QFont>
#include <QJsonValue>
#include <QString>

#include "rec/presentationbackground.h"
#include "rec/textstyle.h"

template<typename T>
inline void loadFromJSON(T &subj, const QJsonValue &json) {
	Q_UNUSED(subj);
	Q_UNUSED(json);
	throw 0;
}

template<typename T>
inline QJsonValue toJSON(const T &subj) {
	Q_UNUSED(subj);
	throw 0;
}

#define JSON_HEADERS(T)                                   \
	template<>                                              \
	void loadFromJSON<T>(T & subj, const QJsonValue &json); \
	template<>                                              \
	QJsonValue toJSON<T>(const T &subj);

JSON_HEADERS(bool)
JSON_HEADERS(int)

JSON_HEADERS(QString)
JSON_HEADERS(QColor)
JSON_HEADERS(QFont)

JSON_HEADERS(TextStyle)
JSON_HEADERS(PresentationBackground)

#undef JSON_HEADERS

#endif// JSONAUTOMATION_H
