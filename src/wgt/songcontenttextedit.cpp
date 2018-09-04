#include "songcontenttextedit.h"

#include "util/songcontentsyntaxhiglighter.h"
#include "rec/chord.h"

SongContentTextEdit::SongContentTextEdit(QWidget *parent) : QTextEdit(parent)
{
	contentSyntaxHiglighter_ = new SongContentSyntaxHiglighter(document());
	viewport()->installEventFilter(this);
}

SongContentSyntaxHiglighter *SongContentTextEdit::syntaxHiglighter()
{
	return contentSyntaxHiglighter_;
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

	if(!isEnabled() || isReadOnly())
		return QTextEdit::keyPressEvent(e);;

	static QHash<QString,QString> autocompleteStrings {
		{"[", "]"},
		{"{", "}"},
		{"(", ")"},
		{"\"", "\""}
	};

	QTextEdit::keyPressEvent(e);

	if(autocompleteStrings.contains(e->text())) {
		const QString str = autocompleteStrings[e->text()];
		auto cursor = textCursor();
		insertPlainText(str);

		cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, str.length());
		setTextCursor(cursor);
	}
}

bool SongContentTextEdit::eventFilter(QObject *obj, QEvent *e)
{
	// On double click on chord, select the entire chord
	if(e->type() == QEvent::MouseButtonDblClick && static_cast<QMouseEvent*>(e)->button() == Qt::LeftButton) {
		auto chs = chordAroundPos(toPlainText(), cursorForPosition(static_cast<QMouseEvent*>(e)->pos()).position());
		if(!chs.chord.isValid())
			return false;

		QTextCursor cursor(document());
		cursor.setPosition(chs.annotationPos);
		cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, chs.annotationLength);
		setTextCursor(cursor);

		return true;
	}

	return QTextEdit::eventFilter(obj, e);
}
