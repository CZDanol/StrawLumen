#include "songcontenttextedit.h"

#include "util/songcontentsyntaxhiglighter.h"

SongContentTextEdit::SongContentTextEdit(QWidget *parent) : QTextEdit(parent)
{
	contentSyntaxHiglighter_ = new SongContentSyntaxHiglighter(document());
}

void SongContentTextEdit::keyPressEvent(QKeyEvent *e)
{
	QTextEdit::keyPressEvent(e);

	static QHash<QString,QString> autocompleteStrings {
		{"[", "]"},
		{"{", "}"},
		{"(", ")"},
		{"\"", "\""}
	};

	if(autocompleteStrings.contains(e->text())) {
		const QString str = autocompleteStrings[e->text()];
		auto cursor = textCursor();
		insertPlainText(str);

		cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, str.length());
		setTextCursor(cursor);
	}
}
