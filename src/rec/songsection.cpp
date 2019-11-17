#include "songsection.h"

SongSection::SongSection()
{
	isValid_ = false;
}

SongSection::SongSection(const QString &str)
{
	static const QRegularExpression regex("^"
																				"(?:"
																				"([VCBIOMP])([1-9][0-9]*)?" // Standard section format
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
	index_ = qMax(1, ma.captured(mpStandardIndex).toInt());
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
	return name_ + QString::number(index_);
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
		{"P", tr("Předrefrén")}
	};

	if(!isValid_)
		return tr("## NEVALIDNÍ ##");

	if(!isStandard_)
		return name_;

	return index_ == 1 ? userFriendlyNames[name_] : tr("%1 %2").arg(userFriendlyNames[name_], QString::number(index_));
}

QString SongSection::shorthandName() const
{
	static QHash<QString,QString> shorthandNames {
		{"C", tr("Ref. %1")},
		{"V", tr("%1.")},
		{"I", tr("Intro %1")},
		{"O", tr("Outro %1")},
		{"B", tr("Bridge %1")},
		{"M", tr("Mezihra %1")},
		{"P", tr("Předrefrén %1")},
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

	return QString("{%1%2}").arg(name_, QString::number(index_));
}

QPixmap SongSection::icon() const
{
	static const QHash<QString, QPixmap> map {
		{"C", QPixmap(":/icons/16/Synchronize_16px.png")},
		{"I", QPixmap(":/icons/16/Curved Arrow_16px.png")},
		{"O", QPixmap(":/icons/16/Right 2_16px.png")},
		{"B", QPixmap(":/icons/16/Circle_16px.png")},
		{"M", QPixmap(":/icons/16/Musical Notes_16px.png")},
		{"P", QPixmap(":/icons/16/up_2_16px.png")},

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

void SongSection::increaseIndex()
{
	index_ ++;
}

QVector<SongSection> songSections(const QString &song)
{
	QVector<SongSection> result;
	QSet<QString> songSectionNames;
	QRegularExpressionMatchIterator it = songSectionAnnotationRegex().globalMatch(song);

	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		SongSection ss(m.captured(2));

		if(!ss.isValid())
			continue;

		while(songSectionNames.contains(ss.standardName()))
			ss.increaseIndex();

		songSectionNames.insert(ss.standardName());

		result.append(ss);
	}

	return result;
}

QVector<SongSectionWithContent> songSectionsWithContent(const QString &song)
{
	static const QRegularExpression rxTrim("^\\s*(.*?)\\s*$", QRegularExpression::DotMatchesEverythingOption);

	// Default - intro
	SongSection currentSection("I");
	QVector<SongSectionWithContent> result;
	QSet<QString> songSectionNames;
	int sectionStart = 0;
	int annotationStart = 0;

	QRegularExpressionMatchIterator it = songSectionAnnotationRegex().globalMatch(song);
	while(it.hasNext()) {
		QRegularExpressionMatch m = it.next();
		SongSection ss(m.captured(2));

		if(!ss.isValid())
			continue;

		while(songSectionNames.contains(ss.standardName()))
			ss.increaseIndex();

		songSectionNames.insert(ss.standardName());

		const QString baseContent = song.mid(sectionStart, m.capturedStart()-sectionStart);
		const auto m2 = rxTrim.match(baseContent);
		const QString content = m2.captured(1);

		if(!content.isEmpty() || sectionStart != 0)
			result.append(SongSectionWithContent{currentSection, content, sectionStart + m2.capturedStart(1), annotationStart, m.capturedStart()});

		currentSection = ss;
		sectionStart = m.capturedEnd();
		annotationStart = m.capturedStart();
	}

	{
		const QString baseContent = song.mid(sectionStart, song.length()-sectionStart);
		const auto m2 = rxTrim.match(baseContent);
		result.append(SongSectionWithContent{currentSection, m2.captured(1), sectionStart + m2.capturedStart(1), annotationStart, song.length()});
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
