#include "multisongselectionwidget.h"
#include "ui_multisongselectionwidget.h"

#include <QShortcut>
#include <QDragEnterEvent>
#include <QMimeData>

#include "gui/splashscreen.h"
#include "job/db.h"
#include "util/standarddialogs.h"

MultiSongSelectionWidget::MultiSongSelectionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MultiSongSelectionWidget)
{
	ui->setupUi(this);
	ui->twSongs->setCornerWidget(ui->twSongsCorner);
	ui->twSelection->setCornerWidget(ui->twSelectionCorner);

	ui->tvSelection->setModel(&selectionModel_);

	connect(&selectionModel_, SIGNAL(sigItemsManipulated(int,int)), this, SLOT(onSelectionItemsManipulated(int,int)));

	// Setup headers
	{
		auto header = ui->tvSelection->header();
		header->setSectionResizeMode(0, QHeaderView::Fixed);
		header->resizeSection(0, 150);
		header->setSectionResizeMode(1, QHeaderView::Stretch);
	}

	// Setup drag & drop
	{
		/*ui->tvSelection->setDragEnterEventHandler([this](QDragEnterEvent *e){
			if(!e->mimeData()->hasFormat("application/straw.lumen.song.ids"))
				return false;

			e->setDropAction(Qt::MoveAction);
			e->accept();
			return true;
		});
		ui->tvSelection->setDropEventHandler([this](QTreeWidgetItem *, int row, const QMimeData* mime, Qt::DropAction){
			if(!mime->hasFormat("application/straw.lumen.song.ids"))
				return false;

			QList<QTreeWidgetItem*> items;

			splashscreen->asyncAction(tr("Přidávání písní"), false, [&]{
				QDataStream stream(mime->data("application/straw.lumen.song.ids"));
				while(!stream.atEnd()) {
					qlonglong songId;
					stream >> songId;
					items << createItem(songId);
				}
			});

			ui->tvSelection->insertTopLevelItems(row, items);

			return true;
		});*/
	}

	// Shortcuts
	{
		connect(new QShortcut(Qt::Key_Delete, ui->tvSelection, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), this, SLOT(deleteSelected()));
	}
}

MultiSongSelectionWidget::~MultiSongSelectionWidget()
{
	delete ui;
}

QVector<qlonglong> MultiSongSelectionWidget::selectedSongs() const
{
	return selectionModel_.items();
}

void MultiSongSelectionWidget::clearSelection()
{
	selectionModel_.clear();
}

void MultiSongSelectionWidget::deleteSelected()
{
	selectionModel_.deleteItems(ui->tvSelection->selectionModel()->selectedRows(0));
}

void MultiSongSelectionWidget::onSelectionItemsManipulated(int index, int count)
{
	auto model = ui->tvSelection->selectionModel();
	model->clear();

	for(int i = 0; i < count; i ++)
		model->select(selectionModel_.index(index+i, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MultiSongSelectionWidget::on_btnAddAll_clicked()
{
	selectionModel_.addItems(ui->wgtSongList->rowIds());
}

void MultiSongSelectionWidget::on_btnClear_clicked()
{
	if(!standardConfirmDialog(tr("Opravdu odebrat všechny položky z výběru?")))
		return;

	selectionModel_.clear();
}

void MultiSongSelectionWidget::on_wgtSongList_sigItemActivated(const qlonglong songId)
{
	selectionModel_.addItem(songId);
}

void MultiSongSelectionWidget::on_tvSelection_activated(const QModelIndex &index)
{
	selectionModel_.deleteItem(selectionModel_.itemAt(index));
}