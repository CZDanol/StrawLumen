#include "bibleverseselectionwidget.h"
#include "ui_bibleverseselectionwidget.h"

#include <QShortcut>
#include <QMenu>

#include "job/db.h"
#include "rec/biblebook.h"
#include "job/settings.h"

int chapterUID(int bookId, int chapterId) {
	return bookId * 1000 + chapterId;
}

int chapterUID_getChapterID(int chapterUID) {
	return chapterUID % 1000;
}

int verseUID(int bookId, int chapterId, int verseId) {
	return chapterUID(bookId, chapterId) * 1000 + verseId;
}

BibleVerseSelectionWidget::BibleVerseSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BibleVerseSelectionWidget)
{
	ui->setupUi(this);

	searchTimer_.setSingleShot(true);
	searchTimer_.setInterval(2000);

	connect(db, &DatabaseManager::sigBibleTranslationsChanged, this, &BibleVerseSelectionWidget::requeryTranslations);
	connect(ui->lnSearch, &QLineEdit::editingFinished, this, &BibleVerseSelectionWidget::updateSearch);
	connect(ui->lnSearch, &QLineEdit::textChanged, &searchTimer_, QOverload<>::of(&QTimer::start));
	connect(&searchTimer_, &QTimer::timeout, this, &BibleVerseSelectionWidget::updateSearch);

	// Models setup
	{
		ui->cmbTranslation->setModel(&translationsModel_);
		ui->lstBooks->setModel(&booksModel_);
		ui->lstChapters->setModel(&chaptersModel_);

		ui->lstVerses->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		ui->lstVerses->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

		connect(ui->lstBooks->selectionModel(), &QItemSelectionModel::currentChanged, this, &BibleVerseSelectionWidget::onBooksModelCurrentIndexChanged);
		connect(ui->lstChapters->selectionModel(), &QItemSelectionModel::currentChanged, this, &BibleVerseSelectionWidget::onChaptersModelCurrentIndexChanged);
		connect(ui->lstVerses->selectionModel(), &QItemSelectionModel::selectionChanged, this, &BibleVerseSelectionWidget::onVersesSelectionChanged);
	}

	// Shortcuts
	{
		new QShortcut(Qt::CTRL | Qt::Key_F, ui->lnSearch, SLOT(setFocus()));
	}

	currentTranslationId_ = settings->defaultBibleTranslation();
	requeryTranslations();
}

BibleVerseSelectionWidget::~BibleVerseSelectionWidget()
{
	delete ui;
}

const BibleRef &BibleVerseSelectionWidget::bibleRef() const
{
	return bibleRef_;
}

void BibleVerseSelectionWidget::setBibleRef(const BibleRef &set, bool updateInputs)
{
	bibleRef_ = set;

	if(updateInputs)
		ui->lnCode->setText(bibleRef_.toString());

	if(blockSelectionChangeEvents_ || !bibleRef_.isValid()) {
		emit sigSelectionChanged();
		return;
	}

	currentTranslationId_ = set.translationId;
	currentBookId_ = set.bookId;
	currentChapterUID_ = chapterUID(currentBookId_, set.chapter);

	selectedVerseUIDs_.clear();
	for(int verse : set.verses())
		selectedVerseUIDs_ += verseUID(currentBookId_, set.chapter, verse);

	requeryTranslations();
	emit sigSelectionChanged();
}

void BibleVerseSelectionWidget::requeryTranslations()
{
	translationIds_.clear();
	translationRows_.clear();
	translationList_.clear();

	QSqlQuery q = db->selectQuery("SELECT translation_id || ' | ' || name, translation_id FROM bible_translations ORDER BY translation_id");
	while(q.next()) {
		const QString translationId = q.value(1).toString();
		translationRows_[translationId] = translationIds_.size();
		translationIds_ += translationId;
		translationList_ += q.value(0).toString();
	}

	blockSelectionChangeEvents_++;
	translationsModel_.setStringList(translationList_);

	if(translationRows_.contains(currentTranslationId_))
		 ui->cmbTranslation->setCurrentIndex(translationRows_[currentTranslationId_]);
	else
		ui->cmbTranslation->setCurrentIndex(translationRows_.value(settings->defaultBibleTranslation(), 0));

	requeryBooks();

	blockSelectionChangeEvents_--;
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
	bool found = false;

	while(booksQuery.next()) {
		const int bookId = booksQuery.value(1).toInt();

		if(bookId == currentBookId_) {
			currentBookRow = booksList_.size();
			found = true;
		}

		booksList_.append(booksQuery.value(0).toString());
		bookIds_.append(bookId);
	}

	if(!found)
		currentBookId_ = -1;

	blockSelectionChangeEvents_++;
	booksModel_.setStringList(booksList_);
	ui->lstBooks->setCurrentIndex(booksModel_.index(currentBookRow));
	blockSelectionChangeEvents_--;

	requeryChapters();
}

void BibleVerseSelectionWidget::requeryChapters()
{
	chaptersList_.clear();
	chapterUIDs_.clear();

	// No book selected -> no chapters
	if(currentBookId_ == -1) {
		chaptersModel_.setStringList(chaptersList_);
		isCurrentChapterSelected_ = false;

		if(isSearch_)
			ui->lstChapters->setCurrentIndex(chaptersModel_.index(0));

		requeryVerses();
		return;
	}

	int currentChapterRow = isSearch_ ? 0 : -1;

	if(isSearch_) {
		chaptersList_ += tr("(vše)");
		chapterUIDs_ += -1;

		isCurrentChapterSelected_ = false;

		QSqlQuery chaptersQuery = db->selectQuery(
			"SELECT chapter FROM bible_verses_fulltext"
			" INNER JOIN bible_translation_verses ON bible_verses_fulltext.rowid = bible_translation_verses.id"
			" WHERE (bible_verses_fulltext MATCH ?) AND (+translation_id = ?) AND (+book_id = ?)"
			" GROUP BY chapter"
			" ORDER BY chapter ASC",
			{searchQuery_, currentTranslationId_, currentBookId_});

		while(chaptersQuery.next()) {
			const int chapterId = chaptersQuery.value(0).toInt();
			const int chapterUid = chapterUID(currentBookId_, chapterId);

			if(chapterUid == currentChapterUID_) {
				currentChapterRow = chaptersList_.size();
				isCurrentChapterSelected_ = true;
			}

			chaptersList_ += QString::number(chapterId);
			chapterUIDs_ += chapterUid;
		}

	} else {
		const int maxChapter = db->selectValue("SELECT max_chapter FROM bible_translation_books WHERE translation_id = ? AND book_id = ?", {currentTranslationId_, currentBookId_}).toInt();

		for(int i = 1; i <= maxChapter; i++) {
			chaptersList_ += QString::number(i);
			chapterUIDs_ += chapterUID(currentBookId_, i);
		}

		if(currentChapterUID_ >= chapterUID(currentBookId_, 1)  && currentChapterUID_ < chapterUID(currentBookId_, maxChapter+1)) {
			currentChapterRow = chapterUID_getChapterID(currentChapterUID_) - 1;
			isCurrentChapterSelected_ = true;
		}
		else {
			currentChapterRow = -1;
			isCurrentChapterSelected_ = false;
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

	// No search and no chapter selected -> no results
	if(!isSearch_ && !isCurrentChapterSelected_)
		return;

	QList<QTreeWidgetItem*> items;
	QList<int> selectedItemRows;

	QSqlQuery versesQuery;
	if(isSearch_) {
		QString queryStr =
				"SELECT book_id, chapter, verse, bible_translation_verses.text FROM bible_verses_fulltext"
				" INNER JOIN bible_translation_verses ON bible_verses_fulltext.docid = bible_translation_verses.id"
				" WHERE (bible_verses_fulltext MATCH ?) AND (+translation_id = ?)%1"
				" ORDER BY book_id ASC, chapter ASC, verse ASC"
				" LIMIT 201";

		QString filters;
		QVariantList args {searchQuery_, currentTranslationId_};

		if(currentBookId_ != -1) {
			filters += " AND (+book_id = ?)";
			args += currentBookId_;
		}

		if(isCurrentChapterSelected_) {
			filters += " AND (+chapter = ?)";
			args += chapterUID_getChapterID(currentChapterUID_);
		}

		versesQuery = db->selectQuery(queryStr.arg(filters),args);

	} else
		versesQuery = db->selectQuery(
					"SELECT book_id, chapter, verse, text"
					" FROM bible_translation_verses"
					" WHERE (translation_id = ?) AND (book_id = ?) AND (chapter = ?)"
					" ORDER BY verse ASC",
					{currentTranslationId_, currentBookId_, chapterUID_getChapterID(currentChapterUID_)}
					);

	size_t i = 0;
	while(versesQuery.next()) {
		const int verseId = versesQuery.value(2).toInt();
		const int bookId = versesQuery.value(0).toInt();
		const int chapterId = versesQuery.value(1).toInt();
		const int verseUid = verseUID(bookId, chapterId, verseId);

		QTreeWidgetItem *it = new QTreeWidgetItem();

		if(isSearch_ && i == 200) {
			it->setText(1, tr("(a další; výsledky omezeny na 200 veršů)"));
			it->setFlags(Qt::ItemNeverHasChildren);

		}
		else {
			if(selectedVerseUIDs_.contains(verseUid))
				selectedItemRows += items.size();

			if(!isCurrentChapterSelected_)
				it->setText(0, QString("%1 %2:%3").arg(getBibleBook(versesQuery.value(0).toInt()).id, versesQuery.value(1).toString(), versesQuery.value(2).toString()));
			else
				it->setText(0, versesQuery.value(2).toString());

			it->setText(1, versesQuery.value(3).toString());

			it->setData(0, Qt::UserRole, verseUid);
			it->setData(0, Qt::UserRole+1, bookId);
			it->setData(0, Qt::UserRole+2, chapterId);
			it->setData(0, Qt::UserRole+3, verseId);
		}

		items.append(it);
		i++;
	}

	blockSelectionChangeEvents_++;

	ui->lstVerses->addTopLevelItems(items);
	ui->lstVerses->setSelectionMode(!isCurrentChapterSelected_ ? QAbstractItemView::SingleSelection : QAbstractItemView::ExtendedSelection);
	ui->lstVerses->selectionModel()->clear();

	for(int ix : selectedItemRows)
		ui->lstVerses->selectionModel()->select(ui->lstVerses->model()->index(ix, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

	blockSelectionChangeEvents_--;
}

void BibleVerseSelectionWidget::updateSearch()
{
	const QString prevSearchQuery = searchQuery_;
	searchQuery_ = collateFulltextQuery(ui->lnSearch->text());
	if(searchQuery_ == prevSearchQuery)
		return;

	isSearch_ = !searchQuery_.isEmpty();
	requeryBooks();
	searchTimer_.stop();
}

void BibleVerseSelectionWidget::onBooksModelCurrentIndexChanged()
{
	if(blockSelectionChangeEvents_)
		return;

	const int currentRow = ui->lstBooks->currentIndex().row();
	currentBookId_ = currentRow == -1 ? -1 : bookIds_[currentRow];
	requeryChapters();
}

void BibleVerseSelectionWidget::onChaptersModelCurrentIndexChanged()
{
	if(blockSelectionChangeEvents_)
		return;

	const int currentRow = ui->lstChapters->currentIndex().row();
	currentChapterUID_ = currentRow == -1 ? -1 : chapterUIDs_[currentRow];
	isCurrentChapterSelected_ = true;
	requeryVerses();
}

void BibleVerseSelectionWidget::onVersesSelectionChanged()
{
	if(blockSelectionChangeEvents_)
		return;

	auto items = ui->lstVerses->selectedItems();
	if(items.isEmpty())
		return;

	auto it = items.front();

	QVector<int> verseIds;
	selectedVerseUIDs_.clear();

	for(QTreeWidgetItem *i : items) {
		verseIds += i->data(0, Qt::UserRole+3).toInt();
		selectedVerseUIDs_ += i->data(0, Qt::UserRole).toInt();
	}

	blockSelectionChangeEvents_++;
	setBibleRef(BibleRef(currentTranslationId_, it->data(0, Qt::UserRole+1).toInt(), it->data(0, Qt::UserRole+2).toInt(), verseIds));
	blockSelectionChangeEvents_--;
}

void BibleVerseSelectionWidget::on_cmbTranslation_currentIndexChanged(int)
{
	if(blockSelectionChangeEvents_)
		return;

	const int row = ui->cmbTranslation->currentIndex();
	if(row == -1)
		return;

	currentTranslationId_ = translationIds_[row];
	blockSelectionChangeEvents_++;
	requeryBooks();
	blockSelectionChangeEvents_--;
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
	currentChapterUID_ = chapterUID(currentBookId_, item->data(0, Qt::UserRole+2).toInt());

	ui->lnSearch->clear();
	searchQuery_.clear();
	isSearch_ = false;
	blockSelectionChangeEvents_ ++;
	requeryBooks();
	blockSelectionChangeEvents_ --;
}

void BibleVerseSelectionWidget::on_lnCode_textChanged(const QString &arg1)
{
	setBibleRef(BibleRef(arg1), false);
}
