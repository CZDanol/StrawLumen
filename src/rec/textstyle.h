#ifndef TEXTSTYLE_H
#define TEXTSTYLE_H

#include <QFont>
#include <QColor>
#include <QTextOption>
#include <QJsonValue>

// F(identifier, capitalizedIdentifier, Type, defaultValue)
#define TEXT_STYLE_FIELD_FACTORY(F)\
	F(font, Font, QFont, QFont("Tahoma", 12))\
	F(color, Color, QColor, Qt::white)\
	\
	F(outlineEnabled, OutlineEnabled, bool, false)\
	F(outlineWidth, OutlineWidth, int, 8)\
	F(outlineColor, OutlineColor, QColor, Qt::black)\
	\
	F(backgroundEnabled, BackgroundEnabled, bool, false)\
	F(backgroundPadding, BackgroundPadding, int, 20)\
	F(backgroundColor, BackgroundColor, QColor, QColor(0,0,0,128))

struct TextStyle {

public:
	enum Flags : int {
		fScaleDownToFitRect = 0b1
	};

public:
	static TextStyle fromJSON(const QJsonValue &json);

public:
#define F(identifier, capitalizedIdentifier, Type, defaultValue) Type identifier = defaultValue;
	TEXT_STYLE_FIELD_FACTORY(F)
#undef F

public:
	void drawText(QPainter &p, const QRect &rect, const QString &str, const QTextOption &option = QTextOption(Qt::AlignCenter), int flags = fScaleDownToFitRect) const;

public:
	void loadFromJSON(const QJsonValue &json);
	QJsonValue toJSON() const;

public:
	bool operator==(const TextStyle &other) const;

};

#endif // TEXTSTYLE_H
