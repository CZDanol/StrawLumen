#include "songsection.h"

SongSection::SongSection()
{
	isValid_ = false;
}

SongSection::SongSection(const QString &str)
{
	static const QRegularExpression regex("^"
																				"(?:"
																				"([VCBIOM])([1-9][0-9]*)?" // Standard section format
																				"|"
																				"\"([a-zA-Z0-9_\\-+]+)\"" // Custom section name
																				")"
																				"$",
																				QRegularExpression::UseUnicodePropertiesOption);

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
	static QHash<QString,QString> userFriendlyNames {
		{"C", tr("Refrén")},
		{"V", tr("Sloka")},
		{"I", tr("Intro")},
		{"O", tr("Outro")},
		{"B", tr("Bridge")},
		{"M", tr("Mezihra")},
	};

	if(!isValid_)
		return tr("## NEVALIDNÍ ##");

	if(!isStandard_)
		return name_;

	return index_.isEmpty() ? userFriendlyNames[name_] : tr("%1 %2").arg(userFriendlyNames[name_], index_);
}

QString SongSection::shorthandName() const
{
	static QHash<QString,QString> shorthandNames {
		{"C", tr("Ref. %1")},
		{"V", tr("%1.")},
		{"I", tr("In. %1")},
		{"O", tr("Out. %1")},
		{"B", tr("Bridge %1")},
		{"M", tr("Mezihra %1")},
	};

	if(!isValid_)
		return tr("## NEVALIDNÍ ##");

	if(!isStandard_)
		return name_;

	return shorthandNames[name_].arg(index_);
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
		{"C", QPixmap(":/icons/16/Synchronize_16px.png")},
		{"I", QPixmap(":/icons/16/Curved Arrow_16px.png")},
		{"O", QPixmap(":/icons/16/Right 2_16px.png")},
		{"B", QPixmap(":/icons/16/Circle_16px.png")},
		{"M", QPixmap(":/icons/16/Musical Notes_16px.png")},

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

	// First, try standard name with index, then without index
	return map.value(standardName(), map.value(name_, QPixmap()));
}

QVector<SongSection> songSections(const QString &song)
{
	QVector<SongSection> result;
	QRegularExpressionMatchIterator it = songSectionAnnotationRegex().globalMatch(song);

	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		SongSection ss(m.captured(2));

		if(!ss.isValid())
			continue;

		result.append(ss);
	}

	return result;
}

QVector<SongSectionWithContent> songSectionsWithContent(const QString &song)
{
	// Default - intro
	SongSection currentSection("I");
	QVector<SongSectionWithContent> result;
	int sectionStart = 0;

	QRegularExpressionMatchIterator it = songSectionAnnotationRegex().globalMatch(song);
	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		SongSection ss(m.captured(2));

		if(!ss.isValid())
			continue;

		QString content = song.mid(sectionStart, m.capturedStart()-sectionStart).trimmed();
		if(!content.isEmpty() || sectionStart != 0)
			result.append(SongSectionWithContent{currentSection, content});

		currentSection = ss;
		sectionStart = m.capturedEnd();
	}

	{
		QString content = song.mid(sectionStart, song.length()-sectionStart).trimmed();
		result.append(SongSectionWithContent{currentSection, content});
	}

	return result;
}

const QRegularExpression &songSectionAnnotationRegex()
{
	static const QRegularExpression result("(\\{)([a-zA-Z0-9\"_\\-+]+)(\\})");
	return result;
}

const QRegularExpression &songSlideSeparatorRegex()
{
	static const QRegularExpression result("\\{---\\}");
	return result;
}

const QRegularExpression &songCustomSlideOrderRegex()
{
	static const QRegularExpression result("^($|[a-zA-Z0-9\\-_]+( [a-zA-Z0-9\\-_]*)*$)");
	return result;
}
