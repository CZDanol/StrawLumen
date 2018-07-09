#include "presentationstyle.h"

#include <QPainter>
#include <QJsonObject>
#include <QJsonDocument>

#include "job/jsonautomation.h"
#include "job/db.h"

PresentationStyle::PresentationStyle(QObject *parent) : QObject(parent)
{
	connect(db, SIGNAL(sigStyleChanged(qlonglong)), this, SLOT(onDbStyleChanged(qlonglong)));
	connect(&background_, SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
}

void PresentationStyle::loadFromJSON(const QJsonValue &val)
{
	const bool oldBlockSignals = blockSignals(true);
	const QJsonObject json = val.toObject();

#define F(identifier, capitalizedIdentifier, Type)\
	::loadFromJSON<Type>(identifier ## _, json[#identifier]);

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	styleId_ = -1;

	blockSignals(oldBlockSignals);
	emit sigChanged();
}

void PresentationStyle::loadFromDb(qlonglong styleId)
{
	const bool oldBlockSignals = blockSignals(true);

	QByteArray data = db->selectValueDef("SELECT data FROM styles WHERE id = ?", {styleId}).toByteArray();
	if(data.isNull())
		return;

	loadFromJSON(QJsonDocument::fromJson(data).object());
	styleId_ = styleId;

	blockSignals(oldBlockSignals);
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

qlonglong PresentationStyle::styleId() const
{
	return styleId_;
}

bool PresentationStyle::operator==(const PresentationStyle &other) const
{
	if(styleId_ != other.styleId_)
		return false;

#define F(identifier, capitalizedIdentifier, Type)\
	if(!(identifier ## _ == other.identifier ## _))
		return false;

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	return true;
}

void PresentationStyle::drawSlide(QPainter &p, const QRect &rect, const QString &text, const QString &title) const
{
	const auto marg = [&](qreal l, qreal t, qreal r, qreal b) {
		return QMargins(l * rect.width() * 0.01, t * rect.height() * 0.01, r * rect.width() * 0.01, b * rect.height() * 0.01);
	};

	background_.draw(p, rect);

	titleTextStyle_.drawText(p, QRect(rect.bottomLeft(), rect.bottomRight()).marginsRemoved(marg(leftPadding(), -titleTextPadding()-bottomPadding(), rightPadding(), bottomPadding())), title, QTextOption(Qt::AlignHCenter | Qt::AlignBottom));

	mainTextStyle_.drawText(p, rect.marginsRemoved(marg(leftPadding(), topPadding(), rightPadding(), titleTextPadding()+bottomPadding())), text);
}

void PresentationStyle::operator=(const PresentationStyle &other)
{
	const bool oldBlockSignals = blockSignals(true);

	styleId_ = other.styleId_;

#define F(identifier, capitalizedIdentifier, Type)\
	identifier ## _ = other.identifier ## _;

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	blockSignals(oldBlockSignals);
	emit sigChanged();
}

void PresentationStyle::onDbStyleChanged(qlonglong styleId)
{
	if(styleId != styleId_)
		return;

	loadFromDb(styleId_);
}

// Field getters/setters implementation
#define F(identifier, capitalizedIdentifier, Type)\
	const Type &PresentationStyle::identifier() const { return identifier ## _; } \
	void PresentationStyle::set ## capitalizedIdentifier(const Type &set)\
	{\
		styleId_ = -1;\
		\
		if(identifier ## _ == set)\
			return;\
		\
		identifier ## _ = set;\
		emit sigChanged();\
	}

PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F
