#include "presentationstyle.h"

#include <QPainter>
#include <QJsonObject>

#include "job/jsonautomation.h"

PresentationStyle::PresentationStyle(QObject *parent) : QObject(parent)
{

}

bool PresentationStyle::usesBackground(const QString &filename)
{
	return background_.filename == filename;
}

void PresentationStyle::loadFromJSON(const QJsonValue &val)
{
	const QJsonObject json = val.toObject();

#define F(identifier, capitalizedIdentifier, Type)\
	::loadFromJSON<Type>(identifier ## _, json[#identifier]);

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	emit sigChanged();
}

QJsonObject PresentationStyle::toJSON() const
{
	QJsonObject json;

#define F(identifier, capitalizedIdentifier, Type)\
	json[#identifier] = ::toJSON<Type>(identifier ## _);

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	return json;
}

void PresentationStyle::drawSlide(QPainter &p, const QRect &rect, const QString &text, const QString &title) const
{
	const int mw = (int)( rect.width() * 0.05 );
	const int mh = (int)( rect.height() * 0.05 );
	const QMargins m(mw,mh,mw,mh);

	background_.draw(p, rect);
	mainTextStyle_.drawText(p, rect.marginsRemoved(m), text);
	titleTextStyle_.drawText(p, rect.marginsRemoved(m), title, QTextOption(Qt::AlignHCenter | Qt::AlignBottom));
}

void PresentationStyle::operator=(const PresentationStyle &other)
{
#define F(identifier, capitalizedIdentifier, Type)\
	identifier ## _ = other.identifier ## _;

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	emit sigChanged();
}

// Field getters/setters implementation
#define F(identifier, capitalizedIdentifier, Type)\
	const Type &PresentationStyle::identifier() const { return identifier ## _; } \
	void PresentationStyle::set ## capitalizedIdentifier(const Type &set)\
	{\
		if(identifier ## _ == set)\
			return;\
		\
		identifier ## _ = set;\
		emit sigChanged();\
	}

PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F
