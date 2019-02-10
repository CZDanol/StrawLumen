#ifndef BIBLEVERSESELECTIONWIDGET_H
#define BIBLEVERSESELECTIONWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QCompleter>
#include <QTreeWidgetItem>

namespace Ui {
	class BibleVerseSelectionWidget;
}

class BibleVerseSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BibleVerseSelectionWidget(QWidget *parent = nullptr);
	~BibleVerseSelectionWidget();

private:
	void requeryBooks();
	void requeryChapters();
	void requeryVerses();

private slots:
	void onBooksModelCurrentIndexChanged();
	void onChaptersModelCurrentIndexChanged();
	void onVersesModelCurrentIndexChanged();

private slots:
	void on_cmbTranslation_currentIndexChanged(int index);
	void on_lnSearch_editingFinished();
	void on_lstVerses_customContextMenuRequested(const QPoint &pos);

	void on_actionGoToChapter_triggered();

private:
	QString searchQuery_;
	bool isSearch_ = false;

private:
	QSqlQueryModel translationsModel_;
	QStringListModel booksModel_, chaptersModel_;
	QStringList booksList_, chaptersList_;
	QVector<int> bookIds_, chapterIds_;

private:
	QString currentTranslationId_;
	int currentBookId_ = -1, currentChapterId_ = -1, currentVerseUID_ = -1;
	size_t blockSelectionChangeEvents_ = 0;

private:
	Ui::BibleVerseSelectionWidget *ui;

};

#endif // BIBLEVERSESELECTIONWIDGET_H
