#pragma once

#include <QListWidget>

class BibleTranslationListBox : public QListWidget {
	Q_OBJECT

public:
	explicit BibleTranslationListBox(QWidget *parent = nullptr);

public:
	void updateList();
	void updateList(const QStringList &selection);

	QStringList selectedTranslations() const;

signals:
	void sigChanged();
};
