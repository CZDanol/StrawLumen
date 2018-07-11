#ifndef PRESENTATIONSTYLE_H
#define PRESENTATIONSTYLE_H

#include <QObject>
#include <QJsonValue>
#include <QJsonObject>

#include "rec/textstyle.h"
#include "rec/presentationbackground.h"

// F(identifier, capitalizedIdentifier, Type, defaultValue)
#define PRESENTATION_STYLE_FIELD_FACTORY(F)\
	F(name, Name, QString, QString())\
	F(mainTextStyle, MainTextStyle, TextStyle, TextStyle())\
	F(titleTextStyle, TitleTextStyle, TextStyle, TextStyle())\
	F(background, Background, PresentationBackground, PresentationBackground())\
	F(topPadding, TopPadding, int, 5) F(bottomPadding, BottomPadding, int, 5) F(leftPadding, LeftPadding, int, 5) F(rightPadding, RightPadding, int, 5) F(titleTextPadding, TitleTextPadding, int, 10)

class PresentationStyle : public QObject
{
	Q_OBJECT

public:
	explicit PresentationStyle(QObject *parent = nullptr);

signals:
	void sigChanged();

public:
	void drawSlide(QPainter &p, const QRect &rect, const QString &text, const QString &title) const;

public:
	void loadFromJSON(const QJsonValue &val);
	void loadFromDb(qlonglong styleId);

	QJsonObject toJSON() const;

	/// Returns -1 if style was not loaded from db or the style has been altered
	qlonglong styleId() const;

public:
	bool operator==(const PresentationStyle &other) const;
	void operator=(const PresentationStyle &other);

private slots:
	void onDbStyleChanged(qlonglong styleId);

private:
	qlonglong styleId_ = -1;

public slots:
	// Field setters
#define F(identifier, capitalizedIdentifier, Type, defaultValue) void set ## capitalizedIdentifier(const Type &set);
	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

public:
		// Field getters
#define F(identifier, capitalizedIdentifier, Type, defaultValue) const Type &identifier() const;
	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

private:
	// Fields
#define F(identifier, capitalizedIdentifier, Type, defaultValue) Type identifier ## _ = defaultValue;
	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

};

#endif // PRESENTATIONSTYLE_H
