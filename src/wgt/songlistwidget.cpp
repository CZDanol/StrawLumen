#include "songlistwidget.h"
#include "ui_songlistwidget.h"

#include <QShortcut>
#include <QShowEvent>

#include "job/db.h"

SongListWidget::SongListWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SongListWidget)
{
	ui->setupUi(this);

	ui->tvSongs->setModel(&songsModel_);
	ui->lvTags->setModel(&tagsModel_);

	typingTimer_.setSingleShot(true);
	typingTimer_.setInterval(500);

	connect(db, SIGNAL(sigSongListChanged()), this, SLOT(requeryTags()));
	connect(db, SIGNAL(sigSongListChanged()), this, SLOT(requery()));

	connect(ui->lnSearch, SIGNAL(editingFinished()), this, SLOT(requeryIfFilterChanged()));
	connect(ui->lnSearch, SIGNAL(textChanged(QString)), &typingTimer_, SLOT(start()));
	connect(&typingTimer_, SIGNAL(timeout()), this, SLOT(requeryIfFilterChanged()));

	connect(ui->tvSongs->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentSongChanged(QModelIndex,QModelIndex)));
	connect(ui->tvSongs->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SIGNAL(sigSelectionChanged()));
	connect(ui->tvSongs, SIGNAL(activated(QModelIndex)), this, SLOT(onSongItemActivated(QModelIndex)));
	connect(ui->tvSongs, &ExtendedTreeView::sigTextTyped, this, &SongListWidget::onTextTypedToList);

	connect(ui->lvTags->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentTagChanged(QModelIndex,QModelIndex)));

	{
		auto sc = new QShortcut(Qt::Key_Escape, ui->lnSearch, nullptr, nullptr, Qt::WidgetWithChildrenShortcut);
		connect(sc, SIGNAL(activated()), this, SLOT(clearFilters()));
		connect(sc, SIGNAL(activatedAmbiguously()), this, SLOT(clearFilters()));
	}
	{
		auto sc = new QShortcut(Qt::CTRL | Qt::Key_F, ui->lnSearch);
		connect(sc, SIGNAL(activated()), ui->lnSearch, SLOT(setFocus()));
		connect(sc, SIGNAL(activated()), ui->lnSearch, SLOT(selectAll()));
	}
}

SongListWidget::~SongListWidget()
{
	delete ui;
}

int SongListWidget::currentRowIndex() const
{
	return ui->tvSongs->currentIndex().row();
}

int SongListWidget::selectedRowCount() const
{
	return ui->tvSongs->selectionModel()->selectedRows().size();
}

qlonglong SongListWidget::currentRowId() const
{
	if(!ui->tvSongs->currentIndex().isValid())
		return -1;

	return songsModel_.record(ui->tvSongs->currentIndex().row()).value("id").toLongLong();
}

QVector<qlonglong> SongListWidget::selectedRowIds() const
{
	QVector<qlonglong> result;

	for(const QModelIndex &index : ui->tvSongs->selectionModel()->selectedRows())
		result.append(songsModel_.record(index.row()).value("id").toLongLong());

	return result;
}

QVector<qlonglong> SongListWidget::rowIds() const
{
	QVector<qlonglong> result;

	for(int i = 0; i < songsModel_.rowCount(); i ++)
		result.append(songsModel_.record(i).value("id").toLongLong());

	return result;
}

void SongListWidget::unselect()
{
	ui->tvSongs->selectionModel()->clear();
	ui->tvSongs->setCurrentIndex(QModelIndex());
}

void SongListWidget::requery()
{
	const int prevRow = ui->tvSongs->currentIndex().row();
	const qlonglong prevSelectId = prevRow < 0 ? -1 : songsModel_.record(prevRow).value("id").toLongLong();

	const bool isTagFilter = ui->lvTags->currentIndex().row() > 0;

	// Query data
	{
		QString joins;
		QStringList filters;
		DBManager::Args args;

		const QString filter = collateFulltextQuery(ui->lnSearch->text());
		currentFilterText_ = ui->lnSearch->text();

		if(!filter.isEmpty()) {
			joins += "INNER JOIN songs_fulltext ON songs.id = songs_fulltext.docid ";
			filters += "(songs_fulltext MATCH ?)";
			args += filter;
		}

		if(isTagFilter) {
			joins += "INNER JOIN song_tags ON songs.id == song_tags.song ";
			filters += "(song_tags.tag = ?)";
			args += tagsModel_.record(ui->lvTags->currentIndex().row()).value("tag");
		}

		const QString query =
				"SELECT songs.id, songs.name AS '%1', songs.author AS '%2' "
				"FROM songs "
				"%3 "
				"%4 "
				"ORDER BY songs.name ASC ";

		songsModel_.setQuery(db->selectQuery(query.arg(tr("Název"), tr("Autor"), joins, filters.size() ? "WHERE " + filters.join(" AND ") : QString()), args));

		while(songsModel_.canFetchMore())
			songsModel_.fetchMore();
	}

	// Setup headers
	{
		auto header = ui->tvSongs->header();
		header->hideSection(0);
		header->setSectionResizeMode(1, QHeaderView::Fixed);
		header->resizeSection(1, 150);
		header->setSectionResizeMode(2, QHeaderView::Stretch);
	}

	// Try reselecting previously selected song
	{
		const qlonglong newSelectId = prevRow < 0 ? -1 : songsModel_.record(prevRow).value("id").toLongLong();
		if(prevSelectId == newSelectId)
			ui->tvSongs->selectionModel()->setCurrentIndex(songsModel_.index(prevRow, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}

	ui->wgtTagDisplayNotice->setVisible(isTagFilter);
	if(isTagFilter)
		ui->lblTagDisplayNotice->setText(tr("Zobrazeny pouze písně se štítkem \"%1\".").arg(tagsModel_.record(ui->lvTags->currentIndex().row()).value("tag").toString()));

	emit sigSelectionChanged();
}

void SongListWidget::requeryTags()
{
	const int prevIndex = ui->lvTags->currentIndex().row();
	const QVariant prevTag = tagsModel_.record(prevIndex).value("tag");

	QString sql = QString("SELECT '%1', '' AS tag "
												"UNION ALL "
												"SELECT tag || ' (' || COUNT(song) || ')', tag "
												"FROM song_tags "
												"GROUP BY tag "
												"ORDER BY tag ASC").arg(tr("-- vše --"));
	QSqlQuery q = db->selectQuery(sql);
	tagsModel_.setQuery(q);

	if(tagsModel_.record(prevIndex).value("tag") == prevTag)
		ui->lvTags->setCurrentIndex(tagsModel_.index(prevIndex,0));
}

void SongListWidget::requeryIfFilterChanged()
{
	if(currentFilterText_ == ui->lnSearch->text())
		return;

	requery();
}

void SongListWidget::selectRow(int rowId)
{
	ui->tvSongs->selectionModel()->select(songsModel_.index(rowId,0), QItemSelectionModel::ClearAndSelect);
}

void SongListWidget::clearFilters()
{
	ui->lnSearch->clear();
	ui->lvTags->setCurrentIndex(tagsModel_.index(0,0));
}

void SongListWidget::setDragEnabled(bool set)
{
	ui->tvSongs->setDragEnabled(set);
}

void SongListWidget::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);

	if(e->type() == QEvent::Show && !e->spontaneous()) {
		requery();
		requeryTags();
	}
}

void SongListWidget::onCurrentSongChanged(const QModelIndex &index, const QModelIndex &prevIndex)
{
	if(!index.isValid())
		emit sigCurrentChanged(-1, prevIndex.row());
	else
		emit sigCurrentChanged(songsModel_.record(index.row()).value("id").toLongLong(), prevIndex.row());
}

void SongListWidget::onCurrentTagChanged(const QModelIndex &index, const QModelIndex &prevIndex)
{
	Q_UNUSED(prevIndex);

	if(!index.isValid())
		return;

	requery();
}

void SongListWidget::onSongItemActivated(const QModelIndex &index)
{
	if(!index.isValid())
		return;

	emit sigItemActivated(songsModel_.record(index.row()).value("id").toLongLong());
}

void SongListWidget::onTextTypedToList(const QString &text)
{
	ui->lnSearch->setFocus();
	ui->lnSearch->setText(text);
}

void SongListWidget::on_tvSongs_customContextMenuRequested(const QPoint &pos)
{
	emit sigCustomContextMenuRequested(ui->tvSongs->viewport()->mapToGlobal(pos));
}

void SongListWidget::on_lnSearch_sigDownPressed()
{
	if(!songsModel_.rowCount())
		return;

	ui->tvSongs->selectionModel()->clear();
	ui->tvSongs->selectionModel()->select(songsModel_.index(0,0), QItemSelectionModel::Clear | QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows);
	ui->tvSongs->setFocus();
}

void SongListWidget::on_tvSongs_sigUpPressed()
{
	if(ui->tvSongs->currentIndex().row() == 0) {
		ui->lnSearch->setFocus();
		ui->lnSearch->selectAll();
	}
}

void SongListWidget::on_lvTags_activated(const QModelIndex &index)
{
	Q_UNUSED(index);

	ui->lnSearch->clear();
	requery();
}

void SongListWidget::on_btnClearTagFilter_clicked()
{
	ui->lvTags->setCurrentIndex(tagsModel_.index(0,0));
}
