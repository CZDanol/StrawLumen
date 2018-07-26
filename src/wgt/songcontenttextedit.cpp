#include "songcontenttextedit.h"

#include "util/songcontentsyntaxhiglighter.h"

SongContentTextEdit::SongContentTextEdit(QWidget *parent) : QTextEdit(parent)
{
	contentSyntaxHiglighter_ = new SongContentSyntaxHiglighter(document());
}

void SongContentTextEdit::keyPressEvent(QKeyEvent *e)
{
	if(e->modifiers() == Qt::AltModifier && e->key() == Qt::Key_Left) {
		emit sigAltlLeftPressed();
		e->ignore();
		return;
	}

	if(e->modifiers() == Qt::AltModifier && e->key() == Qt::Key_Right) {
		emit sigAltRightPressed();
		e->ignore();
		return;
	}

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
