#ifndef BIBLEBOOK_H
#define BIBLEBOOK_H

#include <QHash>
#include <QString>
#include <QStringList>

class BibleBook {

public:
	int numId;
	QString id, name;
	QStringList ids;
};

BibleBook getBibleBook(int numId);
QList<BibleBook> bibleBookList();

#endif// BIBLEBOOK_H
