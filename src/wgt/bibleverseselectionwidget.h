#ifndef BIBLEVERSESELECTIONWIDGET_H
#define BIBLEVERSESELECTIONWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QCompleter>
#include <QTreeWidgetItem>
#include <QTimer>

#include "rec/bibleref.h"

namespace Ui {
	class BibleVerseSelectionWidget;
}

class BibleVerseSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BibleVerseSelectionWidget(QWidget *parent = nullptr);
	~BibleVerseSelectionWidget();

public:
	const BibleRef &bibleRef() const;
	void setBibleRef(const BibleRef &set, bool updateInputs = true);

signals:
	void sigSelectionChanged();

private slots:
	void requeryTranslations();
	void requeryBooks();
	void requeryChapters();
	void requeryVerses();
	void updateSearch();

private slots:
	void onBooksModelCurrentIndexChanged();
	void onChaptersModelCurrentIndexChanged();
	void onVersesSelectionChanged();

private slots:
	void on_cmbTranslation_currentIndexChanged(int index);
	void on_lstVerses_customContextMenuRequested(const QPoint &pos);
	void on_actionGoToChapter_triggered();
	void on_lnCode_textChanged(const QString &arg1);

private:
	BibleRef bibleRef_;

private:
	QString searchQuery_;
	bool isSearch_ = false;

private:
	QStringListModel translationsModel_, booksModel_, chaptersModel_;
	QStringList translationList_, booksList_, chaptersList_;
	QStringList translationIds_;
	QVector<int> bookIds_, chapterUIDs_;
	QHash<QString,int> translationRows_;

private:
	QTimer searchTimer_;
	QString currentTranslationId_;
	int currentBookId_ = -1, currentChapterUID_ = -1;
	bool isCurrentChapterSelected_ = false;
	QSet<int> selectedVerseUIDs_;
	size_t blockSelectionChangeEvents_ = 0;

private:
	Ui::BibleVerseSelectionWidget *ui;

};

#endif // BIBLEVERSESELECTIONWIDGET_H
