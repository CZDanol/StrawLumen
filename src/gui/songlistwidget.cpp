#include "songlistwidget.h"
#include "ui_songlistwidget.h"

#include <QShortcut>
#include <QShowEvent>
#include <QDebug>

#include "job/db.h"

SongListWidget::SongListWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SongListWidget)
{
	ui->setupUi(this);

	ui->tvList->setModel(&model_);

	typingTimer_.setSingleShot(true);
	typingTimer_.setInterval(500);

	connect(ui->lnSearch, SIGNAL(editingFinished()), this, SLOT(requeryIfNecessary()));
	connect(ui->lnSearch, SIGNAL(textChanged(QString)), &typingTimer_, SLOT(start()));
	connect(&typingTimer_, SIGNAL(timeout()), this, SLOT(requeryIfNecessary()));

	connect(ui->tvList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex,QModelIndex)));
	connect(ui->tvList, SIGNAL(activated(QModelIndex)), this, SIGNAL(sigItemActivated()));

	new QShortcut(Qt::Key_Escape, ui->lnSearch, SLOT(clear()), SLOT(clear()), Qt::WidgetShortcut);

	{
		auto sc = new QShortcut(Qt::CTRL | Qt::Key_F, ui->lnSearch);
		connect(sc, SIGNAL(activated()), ui->lnSearch, SLOT(setFocus()));
		connect(sc, SIGNAL(activated()), ui->lnSearch, SLOT(selectAll()));
	}

	requery();
}

SongListWidget::~SongListWidget()
{
	delete ui;
}

int SongListWidget::currentRowId()
{
	return ui->tvList->currentIndex().row();
}

void SongListWidget::unselect()
{
	ui->tvList->selectionModel()->clear();
	ui->tvList->setCurrentIndex(QModelIndex());
}

void SongListWidget::requery()
{
	const int prevRow = ui->tvList->currentIndex().row();
	const qlonglong prevSelectId = prevRow < 0 ? -1 : model_.data(model_.index(prevRow, 0)).toLongLong();

	QString query;
	DBManager::Args args;

	const QString filter = collateFulltextQuery(ui->lnSearch->text());
	currentFilterText_ = ui->lnSearch->text();

	if(!filter.isEmpty()) {
		query = "SELECT songs.id, songs.name AS '%1', songs.author AS '%2' "
						"FROM songs "
						"INNER JOIN songs_fulltext ON songs.id = songs_fulltext.docid "
						"WHERE songs_fulltext MATCH ? "
						"ORDER BY songs.name ASC ";

	} else {
		query = "SELECT id, name AS '%1', author AS '%2' "
						"FROM songs "
						"ORDER BY name ASC ";
	}

	QSqlQuery q(db->database());
	q.prepare(query.arg(tr("Název"), tr("Autor")));

	if(!filter.isEmpty())
		q.bindValue(0, filter);

	if(!q.exec()) {
		qCritical() << q.lastError().text();
		return;
	}

	q.last();

	model_.setQuery(q);

	while(model_.canFetchMore())
		model_.fetchMore();

	ui->tvList->setColumnHidden(0, true);
	ui->tvList->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui->tvList->header()->setSectionResizeMode(2, QHeaderView::Stretch);

	const qlonglong newSelectId = prevRow < 0 ? -1 : model_.data(model_.index(prevRow, 0)).toLongLong();
	if(prevSelectId == newSelectId)
		ui->tvList->selectionModel()->setCurrentIndex(model_.index(prevRow, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void SongListWidget::requeryIfNecessary()
{
	if(currentFilterText_ == ui->lnSearch->text())
		return;

	requery();
}

void SongListWidget::selectRow(int rowId)
{
	ui->tvList->selectionModel()->select(model_.index(rowId,0), QItemSelectionModel::ClearAndSelect);
}

void SongListWidget::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);

	if(e->type() == QEvent::Show && !e->spontaneous())
		requery();
}

void SongListWidget::onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex)
{
	if(!index.isValid())
		emit sigSelectionChanged(-1, prevIndex.row());
	else
		emit sigSelectionChanged(model_.data(model_.index(index.row(), 0)).toLongLong(), prevIndex.row());
}

void SongListWidget::onCustomContextMenuRequested(const QPoint &pos)
{
	emit sigCustomContextMenuRequested(ui->tvList->viewport()->mapToGlobal(pos));
}
