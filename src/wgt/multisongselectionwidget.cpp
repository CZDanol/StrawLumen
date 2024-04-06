#include "multisongselectionwidget.h"
#include "ui_multisongselectionwidget.h"

#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QShortcut>

#include "gui/splashscreen.h"
#include "job/db.h"
#include "util/standarddialogs.h"

MultiSongSelectionWidget::MultiSongSelectionWidget(QWidget *parent) : QWidget(parent),
                                                                      ui(new Ui::MultiSongSelectionWidget) {
	ui->setupUi(this);
	ui->twSongs->setCornerWidget(ui->twSongsCorner);
	ui->twSelection->setCornerWidget(ui->twSelectionCorner);

	ui->tvSelection->setModel(&selectionModel_);
	ui->tvSelection->setStartDragFunction([=](Qt::DropActions) {
		const QModelIndexList selection = ui->tvSelection->selectionModel()->selectedIndexes();

		if(selection.isEmpty())
			return false;

		//QMimeData *mime = new QMimeData;
		QMimeData *mime = selectionModel_.mimeData(selection);
		mime->setData("application/straw.lumen.song.null", QByteArray());

		const QPixmap pixmap = selection.first().data(Qt::DecorationRole).value<QPixmap>();

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mime);
		drag->setPixmap(pixmap);

		if(drag->exec(Qt::LinkAction, Qt::LinkAction) != Qt::IgnoreAction && drag->target()->parent() != ui->tvSelection)
			deleteSelected();

		return true;
	});

	connect(&selectionModel_, SIGNAL(sigItemsManipulated(int, int)), this, SLOT(onSelectionItemsManipulated(int, int)));
	connect(&selectionModel_, &SongRecordItemModel::modelReset, this, &MultiSongSelectionWidget::sigSelectionChanged);
	connect(&selectionModel_, &SongRecordItemModel::sigItemsManipulated, this, &MultiSongSelectionWidget::sigSelectionChanged);

	// Setup headers
	{
		auto header = ui->tvSelection->header();
		header->setSectionResizeMode(0, QHeaderView::Fixed);
		header->resizeSection(0, 150);
		header->setSectionResizeMode(1, QHeaderView::Stretch);
	}

	// Shortcuts
	{
		connect(new QShortcut(Qt::Key_Delete, ui->tvSelection, nullptr, nullptr, Qt::WidgetWithChildrenShortcut), SIGNAL(activated()), this, SLOT(deleteSelected()));
	}
}

MultiSongSelectionWidget::~MultiSongSelectionWidget() {
	delete ui;
}

void MultiSongSelectionWidget::setDb(DBManager *mgr, bool allowEdit) {
	selectionModel_.setDb(mgr);
	selectionModel_.clear();

	ui->wgtSongList->setDb(mgr, allowEdit);
	ui->wgtSongList->requeryTags();
	ui->wgtSongList->requery();
}

void MultiSongSelectionWidget::setSelectedSongs(const QVector<qlonglong> &songIds) {
	selectionModel_.clear();
	selectionModel_.addItems(songIds);
}

void MultiSongSelectionWidget::setSelectedSongsIfReasonable(const QVector<qlonglong> &songIds) {
	// If the new selection is subset of the current one, keep the current one (and its order)
	//if(QSet<qlonglong>::fromList(selectionModel_.items().toList()).contains(QSet<qlonglong>::fromList(songIds.toList())))
	//	return;

	// If the current selection model has more than one song, don't change it
	if(songIds.count() <= 1 && selectionModel_.items().count() > 1)
		return;

	setSelectedSongs(songIds);
}

QVector<qlonglong> MultiSongSelectionWidget::selectedSongs() const {
	return selectionModel_.items();
}

bool MultiSongSelectionWidget::isAnySongSelected() const {
	return selectionModel_.rowCount(QModelIndex()) > 0;
}

void MultiSongSelectionWidget::focusSongList() {
	ui->wgtSongList->focusSongList();
}

void MultiSongSelectionWidget::clearSelection() {
	selectionModel_.clear();
}

void MultiSongSelectionWidget::deleteSelected() {
	selectionModel_.deleteItems(ui->tvSelection->selectionModel()->selectedRows(0));
}

void MultiSongSelectionWidget::onSelectionItemsManipulated(int index, int count) {
	auto model = ui->tvSelection->selectionModel();
	model->clear();

	for(int i = 0; i < count; i++)
		model->select(selectionModel_.index(index + i, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void MultiSongSelectionWidget::on_btnAddAll_clicked() {
	selectionModel_.addItems(ui->wgtSongList->rowIds());
}

void MultiSongSelectionWidget::on_btnClear_clicked() {
	if(!standardConfirmDialog(tr("Opravdu odebrat všechny položky z výběru?")))
		return;

	selectionModel_.clear();
}

void MultiSongSelectionWidget::on_wgtSongList_sigItemActivated(const qlonglong songId) {
	selectionModel_.addItem(songId);
}

void MultiSongSelectionWidget::on_tvSelection_activated(const QModelIndex &index) {
	selectionModel_.deleteItem(selectionModel_.itemAt(index));
}

void MultiSongSelectionWidget::on_btnRemoveAll_clicked() {
	selectionModel_.deleteItems(ui->wgtSongList->rowIds());
}
