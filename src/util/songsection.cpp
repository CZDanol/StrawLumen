#include "songsection.h"

#include <QRegularExpression>
#include <QSet>

QVector<SongSection> SongSection::songSections(const QString &song)
{
	static const QRegularExpression regex("\\{([^}]+)\\}");

	QVector<SongSection> result;
	QSet<QString> uniqueSections;
	QRegularExpressionMatchIterator it = regex.globalMatch(song);

	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		SongSection ss(m.captured(1));

		if(!ss.isValid() || uniqueSections.contains(ss.standardName()))
			continue;

		uniqueSections.insert(ss.standardName());
		result.append(ss);
	}

	return result;
}

SongSection::SongSection()
{
	isValid_ = false;
}

SongSection::SongSection(const QString &str)
{
	static const QRegularExpression regex("^"
																				"(?:"
																				"([VCBIO])([1-9][0-9]*)?" // Standard section format
																				"|"
																				"\"([^\"]+)\"" // Custom section name
																				")"
																				"$", QRegularExpression::UseUnicodePropertiesOption);

	enum MatchPart : int {
		mpWhole,
		mpStandardBase,
		mpStandardIndex,
		mpCustomName
	};

	QRegularExpressionMatch ma = regex.match(str);

	isValid_ = ma.hasMatch();
	isStandard_ = ma.capturedLength(mpStandardBase);
	name_ = isStandard_ ? ma.captured(mpStandardBase) : ma.captured(mpCustomName);
	index_ = ma.captured(mpStandardIndex);
}

SongSection SongSection::customSection(const QString &str)
{
	SongSection result;
	result.isValid_ = true;
	result.name_ = str;
	result.isStandard_ = false;
	return result;
}

bool SongSection::isValid() const
{
	return isValid_;
}

QString SongSection::standardName() const
{
	return name_ + index_;
}

QString SongSection::userFriendlyName() const
{
	static QHash<QString,const char*> userFriendlyNames {
		{"C", QT_TR_NOOP("Refrén")},
		{"V", QT_TR_NOOP("Sloka")},
		{"I", QT_TR_NOOP("Intro")},
		{"O", QT_TR_NOOP("Outro")},
		{"B", QT_TR_NOOP("Bridge")},
	};

	if(!isValid_)
		return tr("## NEVALIDNÍ ##");

	if(!isStandard_)
		return name_;

	return index_.isEmpty() ? tr(userFriendlyNames[name_]) : tr("%1 %2").arg(tr(userFriendlyNames[name_]), index_);
}

QString SongSection::annotation() const
{
	if(!isValid_)
		return QString();

	if(!isStandard_)
		return QString("{\"%1\"}").arg(name_);

	return QString("{%1%2}").arg(name_, index_);
}

QPixmap SongSection::icon() const
{
	static const QHash<QString, QPixmap> map {
		{"C", QPixmap(":/icons/16/Repeat_16px.png")},
		{"C1", QPixmap(":/icons/16/Repeat_16px.png")},

		{"I", QPixmap(":/icons/16/Curved Arrow_16px.png")},
		{"O", QPixmap(":/icons/16/Right 2_16px.png")},
		{"B", QPixmap(":/icons/16/Circle_16px.png")},

		{"V", QPixmap(":/icons/16/Level 1_16px.png")},
		{"V1", QPixmap(":/icons/16/Level 1_16px.png")},
		{"V2", QPixmap(":/icons/16/Circled 2 C_16px.png")},
		{"V3", QPixmap(":/icons/16/Circled 3 C_16px.png")},
		{"V4", QPixmap(":/icons/16/Circled 4 C_16px.png")},
		{"V5", QPixmap(":/icons/16/Circled 5 C_16px.png")},
		{"V6", QPixmap(":/icons/16/Circled 6 C_16px.png")}

	};

	static const QPixmap customSectionPixmap(":/icons/16/Quote Left_16px.png");

	if(!isStandard_)
		return customSectionPixmap;

	return map.value(standardName(), QPixmap());
}
