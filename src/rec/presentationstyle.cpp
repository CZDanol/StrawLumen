#include "presentationstyle.h"

#include <QPainter>
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>

#include "job/jsonautomation.h"
#include "job/db.h"

PresentationStyle::PresentationStyle()
{
	moveToThread(QApplication::instance()->thread());

	connect(db, SIGNAL(sigStyleChanged(qlonglong)), this, SLOT(onDbStyleChanged(qlonglong)));
	connect(this, &PresentationStyle::sigChanged, this, &PresentationStyle::sigNeedsRepaint);

	connect(&background_, SIGNAL(sigChanged()), this, SIGNAL(sigChanged()));
	connect(&background_, &PresentationBackground::sigNeedsRepaint, this, &PresentationStyle::sigNeedsRepaint);
}

void PresentationStyle::loadFromJSON(const QJsonValue &val)
{
	QSignalBlocker sb(this);
	const QJsonObject json = val.toObject();

#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
	::loadFromJSON<Type>(identifier ## _, json[#identifier]);

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	styleId_ = -1;
	customBackground_ = false;

	sb.unblock();
	emit sigChanged();
}

void PresentationStyle::loadFromDb(qlonglong styleId)
{
	QSignalBlocker sb(this);

	QByteArray data = db->selectValueDef("SELECT data FROM styles WHERE id = ?", {styleId}).toByteArray();
	if(data.isNull())
		return;

	loadFromJSON(QJsonDocument::fromJson(data).object());
	styleId_ = styleId;
	customBackground_ = false;

	sb.unblock();
	emit sigChanged();
}

bool PresentationStyle::hasCustomBackground() const
{
	return customBackground_;
}

QJsonObject PresentationStyle::toJSON() const
{
	QJsonObject json;

#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
	json[#identifier] = ::toJSON<Type>(identifier ## _);

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

			return json;
}

qlonglong PresentationStyle::styleId() const
{
	return styleId_;
}

void PresentationStyle::assumeStyleId(qlonglong set)
{
	styleId_ = set;
	customBackground_ = false;
}

bool PresentationStyle::operator==(const PresentationStyle &other) const
{
	if(styleId_ != other.styleId_)
		return false;

#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
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
	QSignalBlocker sb(this);

	styleId_ = other.styleId_;
	customBackground_ = other.customBackground_;

#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
	identifier ## _ = other.identifier ## _;

	PRESENTATION_STYLE_FIELD_FACTORY(F)
#undef F

	sb.unblock();
	emit sigChanged();
}

void PresentationStyle::onDbStyleChanged(qlonglong styleId)
{
	if(styleId != styleId_)
		return;

	PresentationBackground bg;
	const bool hadCustomBaackground = hasCustomBackground();
	if(hadCustomBaackground)
		bg = background();

	loadFromDb(styleId_);

	if(hadCustomBaackground)
		setBackground(bg);
}

// Field getters/setters implementation
#define F(identifier, capitalizedIdentifier, Type, defaultValue)\
	const Type &PresentationStyle::identifier() const { return identifier ## _; } \
	void PresentationStyle::set ## capitalizedIdentifier(const Type &set)\
	{\
		if(QString(#identifier) == "background")\
			customBackground_ = true;\
		else\
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
