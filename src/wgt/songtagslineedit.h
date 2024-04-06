#ifndef SONGTAGSLINEEDIT_H
#define SONGTAGSLINEEDIT_H

#include <QCompleter>
#include <QSet>
#include <QSqlQueryModel>

#include "wgt/wordcompletinglineedit.h"

class SongTagsLineEdit : public WordCompletingLineEdit {
	Q_OBJECT

public:
	explicit SongTagsLineEdit(QWidget *parent = nullptr);

public:
	QSet<QString> toTags() const;

protected:
	void showEvent(QShowEvent *e) override;

private slots:
	void updateTagList();

private:
	QCompleter completer_;
	QSqlQueryModel completerModel_;
};

#endif// SONGTAGSLINEEDIT_H
