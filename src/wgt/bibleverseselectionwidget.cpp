#include "bibleverseselectionwidget.h"
#include "ui_bibleverseselectionwidget.h"

#include <QShortcut>
#include <QMenu>

#include "job/db.h"
#include "rec/biblebook.h"

BibleVerseSelectionWidget::BibleVerseSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BibleVerseSelectionWidget)
{
	ui->setupUi(this);

	// Models setup
	{
		translationsModel_.setQuery(db->selectQuery("SELECT translation_id || ' | ' || name, translation_id FROM bible_translations ORDER BY translation_id"));
		ui->cmbTranslation->setModel(&translationsModel_);

		ui->lstBooks->setModel(&booksModel_);
		ui->lstChapters->setModel(&chaptersModel_);
		ui->lstVerses->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		ui->lstVerses->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

		connect(ui->lstBooks->selectionModel(), &QItemSelectionModel::currentChanged, this, &BibleVerseSelectionWidget::onBooksModelCurrentIndexChanged);
		connect(ui->lstChapters->selectionModel(), &QItemSelectionModel::currentChanged, this, &BibleVerseSelectionWidget::onChaptersModelCurrentIndexChanged);
		connect(ui->lstVerses->selectionModel(), &QItemSelectionModel::currentChanged, this, &BibleVerseSelectionWidget::onVersesModelCurrentIndexChanged);
	}

	// Shortcuts
	{
		new QShortcut(Qt::CTRL | Qt::Key_F, ui->lnSearch, SLOT(setFocus()));
	}
}

BibleVerseSelectionWidget::~BibleVerseSelectionWidget()
{
	delete ui;
}

void BibleVerseSelectionWidget::requeryBooks()
{
	booksList_.clear();
	bookIds_.clear();

	QSqlQuery booksQuery;
	if(isSearch_) {
		booksList_ += tr("(vše)");
		bookIds_ += -1;

		booksQuery = db->selectQuery(
					"SELECT name, book_id"
					" FROM bible_translation_books"
					" WHERE book_id IN ("
					"  SELECT DISTINCT book_id"
					"  FROM bible_verses_fulltext"
					"  INNER JOIN bible_translation_verses ON bible_verses_fulltext.rowid = bible_translation_verses.id"
					"  WHERE (bible_verses_fulltext MATCH ?) AND (+translation_id = ?)"
					" ) AND (translation_id = ?)"
					" ORDER BY bible_translation_books.book_id ASC",
					{searchQuery_, currentTranslationId_, currentTranslationId_}
					);
	} else
		booksQuery = db->selectQuery("SELECT name, book_id FROM bible_translation_books WHERE translation_id = ? ORDER BY book_id ASC", {currentTranslationId_});

	int currentBookRow = isSearch_ ? 0 : -1;

	while(booksQuery.next()) {
		const int bookId = booksQuery.value(1).toInt();

		if(bookId == currentBookId_)
			currentBookRow = booksList_.size();

		booksList_.append(booksQuery.value(0).toString());
		bookIds_.append(bookId);
	}

	blockSelectionChangeEvents_++;
	booksModel_.setStringList(booksList_);
	ui->lstBooks->setCurrentIndex(booksModel_.index(currentBookRow));
	blockSelectionChangeEvents_--;

	requeryChapters();
}

void BibleVerseSelectionWidget::requeryChapters()
{
	chaptersList_.clear();
	chapterIds_.clear();

	// No book selected -> no chapters
	if(currentBookId_ == -1) {
		currentChapterId_ = -1;
		chaptersModel_.setStringList(chaptersList_);

		if(isSearch_)
			ui->lstChapters->setCurrentIndex(chaptersModel_.index(0));

		requeryVerses();
		return;
	}

	int currentChapterRow = isSearch_ ? 0 : -1;

	if(isSearch_) {
		chaptersList_ += tr("(vše)");
		chapterIds_ += -1;

		QSqlQuery chaptersQuery = db->selectQuery(
			"SELECT chapter FROM bible_verses_fulltext"
			" INNER JOIN bible_translation_verses ON bible_verses_fulltext.rowid = bible_translation_verses.id"
			" WHERE (bible_verses_fulltext MATCH ?) AND (+translation_id = ?) AND (+book_id = ?)"
			" GROUP BY chapter"
			" ORDER BY chapter ASC",
			{searchQuery_, currentTranslationId_, currentBookId_});

		while(chaptersQuery.next()) {
			const int chapterId = chaptersQuery.value(0).toInt();

			if(chapterId == currentChapterId_)
				currentChapterRow = chaptersList_.size();

			chaptersList_.append(QString::number(chapterId));
			chapterIds_.append(chapterId);
		}

	} else {
		const int maxChapter = db->selectValue("SELECT max_chapter FROM bible_translation_books WHERE translation_id = ? AND book_id = ?", {currentTranslationId_, currentBookId_}).toInt();

		for(int i = 1; i < maxChapter; i++) {
			chaptersList_ += QString::number(i);
			chapterIds_ += i;
		}

		if(currentChapterId_ > 0 && currentChapterId_ <= maxChapter)
			currentChapterRow = currentChapterId_ - (isSearch_ ? 0 : 1);
		else {
			currentChapterRow = 0;
			currentChapterId_ = -1;
		}
	}

	blockSelectionChangeEvents_++;
	chaptersModel_.setStringList(chaptersList_);
	ui->lstChapters->setCurrentIndex(chaptersModel_.index(currentChapterRow));
	blockSelectionChangeEvents_--;

	requeryVerses();
}

void BibleVerseSelectionWidget::requeryVerses()
{
	blockSelectionChangeEvents_++;
	ui->lstVerses->clear();
	blockSelectionChangeEvents_--;

	QTreeWidgetItem *currentVerseItem = nullptr;

	// No search and no chapter selected -> no results
	if(!isSearch_ && currentChapterId_ == -1)
		return;

	QList<QTreeWidgetItem*> items;

	QSqlQuery versesQuery;
	if(isSearch_) {
		QString queryStr =
				"SELECT book_id, chapter, verse, bible_translation_verses.text FROM bible_verses_fulltext"
				" INNER JOIN bible_translation_verses ON bible_verses_fulltext.docid = bible_translation_verses.id"
				" WHERE (bible_verses_fulltext MATCH ?) AND (+translation_id = ?)%1"
				" ORDER BY book_id ASC, chapter ASC, verse ASC"
				" LIMIT 100";

		QString filters;
		QVariantList args {searchQuery_, currentTranslationId_};

		if(currentBookId_ != -1) {
			filters += " AND (+book_id = ?)";
			args += currentBookId_;
		}

		if(currentChapterId_ != -1) {
			filters += " AND (+chapter = ?)";
			args += currentChapterId_;
		}

		versesQuery = db->selectQuery(queryStr.arg(filters),args);

	} else
		versesQuery = db->selectQuery(
					"SELECT book_id, chapter, verse, text"
					" FROM bible_translation_verses"
					" WHERE (translation_id = ?) AND (book_id = ?) AND (chapter = ?)"
					" ORDER BY verse ASC",
					{currentTranslationId_, currentBookId_, currentChapterId_}
					);

	while(versesQuery.next()) {
		const int verseId = versesQuery.value(2).toInt();
		const int bookId = versesQuery.value(0).toInt();
		const int chapterId = versesQuery.value(1).toInt();
		const int verseUID = verseId + chapterId * 1000 + bookId * 1000000;

		QTreeWidgetItem *it = new QTreeWidgetItem();

		if(verseUID == currentVerseUID_)
			currentVerseItem = it;

		if(currentChapterId_ == -1)
			it->setText(0, QString("%1 %2:%3").arg(getBibleBook(versesQuery.value(0).toInt()).id, versesQuery.value(1).toString(), versesQuery.value(2).toString()));
		else
			it->setText(0, versesQuery.value(2).toString());

		it->setText(1, versesQuery.value(3).toString());

		it->setData(0, Qt::UserRole, verseUID);
		it->setData(0, Qt::UserRole+1, bookId);
		it->setData(0, Qt::UserRole+2, chapterId);
		it->setData(0, Qt::UserRole+3, verseId);

		items.append(it);
	}

	blockSelectionChangeEvents_++;
	ui->lstVerses->addTopLevelItems(items);
	ui->lstVerses->setCurrentItem(currentVerseItem);
	blockSelectionChangeEvents_--;
}

void BibleVerseSelectionWidget::onBooksModelCurrentIndexChanged()
{
	if(blockSelectionChangeEvents_)
		return;

	const int currentRow = ui->lstBooks->currentIndex().row();
	currentBookId_ = currentRow == -1 ? -1 : bookIds_[currentRow];
	currentChapterId_ = -1;
	currentVerseUID_ = -1;
	requeryChapters();
}

void BibleVerseSelectionWidget::onChaptersModelCurrentIndexChanged()
{
	if(blockSelectionChangeEvents_)
		return;

	const int currentRow = ui->lstChapters->currentIndex().row();
	currentChapterId_ = currentRow == -1 ? -1 : chapterIds_[currentRow];
	currentVerseUID_ = -1;
	requeryVerses();
}

void BibleVerseSelectionWidget::onVersesModelCurrentIndexChanged()
{
	if(blockSelectionChangeEvents_)
		return;

	const QTreeWidgetItem *it = ui->lstVerses->currentItem();
	currentVerseUID_ = it ? it->data(0, Qt::UserRole).toInt() : -1;
}

void BibleVerseSelectionWidget::on_cmbTranslation_currentIndexChanged(int)
{
	const int row = ui->cmbTranslation->currentIndex();
	if(row == -1)
		return;

	currentTranslationId_ = translationsModel_.record(row).value(1).toString();
	blockSelectionChangeEvents_++;
	requeryBooks();
	blockSelectionChangeEvents_--;
}

void BibleVerseSelectionWidget::on_lnSearch_editingFinished()
{
	const QString prevSearchQuery = searchQuery_;
	searchQuery_ = collateFulltextQuery(ui->lnSearch->text());
	if(searchQuery_ == prevSearchQuery)
		return;

	isSearch_ = !searchQuery_.isEmpty();
	requeryBooks();
}

void BibleVerseSelectionWidget::on_lstVerses_customContextMenuRequested(const QPoint &pos)
{
	QTreeWidgetItem *item = ui->lstVerses->currentItem();
	if(!item || !isSearch_)
		return;

	QMenu m;
	m.addAction(ui->actionGoToChapter);
	m.exec(ui->lstVerses->viewport()->mapToGlobal(pos));
}

void BibleVerseSelectionWidget::on_actionGoToChapter_triggered()
{
	QTreeWidgetItem *item = ui->lstVerses->currentItem();
	if(!item)
		return;

	currentBookId_ = item->data(0, Qt::UserRole+1).toInt();
	currentChapterId_ = item->data(0, Qt::UserRole+2).toInt();
	ui->lnSearch->clear();
	searchQuery_.clear();
	isSearch_ = false;
	blockSelectionChangeEvents_ ++;
	requeryBooks();
	blockSelectionChangeEvents_ --;
}
