#ifndef PRESENTATIONSTYLE_H
#define PRESENTATIONSTYLE_H

#include <QObject>
#include <QJsonValue>
#include <QJsonObject>

#include "rec/textstyle.h"

// F(identifier, capitalizedIdentifier, Type)
#define PRESENTATION_STYLE_FIELD_FACTORY(F)\
	F(name, Name, QString)\
	F(mainTextStyle, MainTextStyle, TextStyle)\
	F(titleTextStyle, TitleTextStyle, TextStyle)

class PresentationStyle : public QObject
{
	Q_OBJECT

public:
	explicit PresentationStyle(QObject *parent = nullptr);

signals:
	void sigChanged();

public slots:
	// Field setters
#define F(identifier, capitalizedIdentifier, Type) void set ## capitalizedIdentifier(const Type &set);
	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

public:
		// Field getters
#define F(identifier, capitalizedIdentifier, Type) const Type &identifier() const;
	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

public:
	void loadFromJSON(const QJsonValue &val);
	QJsonObject toJSON() const;

public:
	void drawSlide(QPainter &p, const QRect &rect, const QString &text, const QString &title, const QString &author) const;

public:
	void operator=(const PresentationStyle &other);

private:
	// Fields
#define F(identifier, capitalizedIdentifier, Type) Type identifier ## _;
	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

};

#endif // PRESENTATIONSTYLE_H
