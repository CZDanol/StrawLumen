#include "songtagslineedit.h"

#include "job/db.h"

SongTagsLineEdit::SongTagsLineEdit(QWidget *parent) : WordCompletingLineEdit(parent)
{
	completer_.setModel(&completerModel_);
	completer_.setCaseSensitivity(Qt::CaseInsensitive);

	setCompleter(&completer_);
	setCompleterSuffix(", ");
	setPlaceholderText(tr("Oddělené čárkou"));
}

QSet<QString> SongTagsLineEdit::toTags() const
{
	QSet<QString> result;
	for(QString tag : text().split(",")) {
		tag = tag.trimmed();

		if(tag.isEmpty() || result.contains(tag))
			continue;

		result.insert(tag);
	}

	return result;
}

void SongTagsLineEdit::focusInEvent(QFocusEvent *e)
{
	completerModel_.setQuery(db->selectQuery("SELECT DISTINCT tag FROM song_tags ORDER BY tag ASC"));

	WordCompletingLineEdit::focusInEvent(e);
}
