#include "jsonautomation.h"

#include <QJsonObject>

#include "job/fontdatabase.h"

#define JSON_FROM(T) template<> void loadFromJSON<T>(T& subj, const QJsonValue &json)
#define JSON_TO(T) template<> QJsonValue toJSON<T>(const T& subj)

JSON_FROM(bool) {
	subj = json.toBool();
}
JSON_TO(bool) {
	return subj;
}

JSON_FROM(int) {
	subj = json.toInt();
}
JSON_TO(int) {
	return subj;
}

JSON_FROM(QString) {
	subj = json.toString();
}
JSON_TO(QString) {
	return subj;
}

JSON_FROM(QColor) {
	subj = QColor(json.toString());
}
JSON_TO(QColor) {
	return subj.name(QColor::HexArgb);
}

JSON_FROM(QFont) {
	subj = fontDatabase().font(json["family"].toString(), json["style"].toString(), json["size"].toInt());
}
JSON_TO(QFont) {
	QJsonObject json;

	json["family"] = subj.family();
	json["style"] = subj.styleName();
	json["size"] = subj.pointSize();

	return json;
}


JSON_FROM(TextStyle) {
	subj.loadFromJSON(json);
}
JSON_TO(TextStyle) {
	return subj.toJSON();
}

JSON_FROM(PresentationBackground) {
	subj.loadFromJSON(json);
}
JSON_TO(PresentationBackground) {
	return subj.toJSON();
}
