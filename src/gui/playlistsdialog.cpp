#include "playlistsdialog.h"
#include "ui_playlistsdialog.h"

#include <QJsonDocument>

#include "gui/mainwindow.h"
#include "gui/mainwindow_presentationmode.h"
#include "job/db.h"
#include "rec/playlist.h"
#include "presentation/presentation.h"
#include "util/standarddialogs.h"

PlaylistsDialog::PlaylistsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PlaylistsDialog)
{
	ui->setupUi(this);

	ui->lstItems->setModel(&itemsModel_);
}

PlaylistsDialog::~PlaylistsDialog()
{
	delete ui;
}

void PlaylistsDialog::show()
{
	requery();
	ui->lstPlaylists->setCurrentRow(0);
	updateUiEnabled();
	QDialog::show();
}

void PlaylistsDialog::updateUiEnabled()
{
	const QListWidgetItem *i = ui->lstPlaylists->currentItem();
	const bool isEditableItem = i && (i->flags() & Qt::ItemIsEditable);

	ui->btnRename->setEnabled(isEditableItem);
	ui->btnDelete->setEnabled(isEditableItem);
	ui->btnLoad->setEnabled(isEditableItem);
	ui->btnSave->setEnabled(isEditableItem);
}

void PlaylistsDialog::requery()
{
	ui->lstPlaylists->clear();

	{
		QListWidgetItem *i = new QListWidgetItem();
		i->setText(tr("(aktuální program)"));
		i->setData(Qt::UserRole, -1);
		ui->lstPlaylists->addItem(i);
	}

	QSqlQuery q = db->selectQuery("SELECT name, id FROM playlists ORDER BY name ASC");
	while(q.next()) {
		QListWidgetItem *i = new QListWidgetItem();
		i->setText(q.value("name").toString());
		i->setData(Qt::UserRole, q.value("id"));
		i->setFlags(i->flags() | Qt::ItemIsEditable);
		ui->lstPlaylists->addItem(i);
	}
}

PlaylistsDialog *playlistsDialog()
{
	static PlaylistsDialog *dlg = nullptr;
	if(!dlg)
		dlg = new PlaylistsDialog(mainWindow);

	return dlg;
}

void PlaylistsDialog::on_btnNew_clicked()
{
	const QString playlistName = tr("Nový program", "Newly created playlist name");
	const QVariant playlistId = db->insert("INSERT INTO playlists(name) VALUES(?)", {playlistName});

	{
		QListWidgetItem *i = new QListWidgetItem();
		i->setText(playlistName);
		i->setData(Qt::UserRole, playlistId);
		i->setFlags(i->flags() | Qt::ItemIsEditable);
		ui->lstPlaylists->addItem(i);
		ui->lstPlaylists->editItem(i);
	}
}

void PlaylistsDialog::on_lstPlaylists_itemChanged(QListWidgetItem *item)
{
	db->exec("UPDATE playlists SET name = ? WHERE id = ?", {item->data(Qt::DisplayRole), item->data(Qt::UserRole)});
}

void PlaylistsDialog::on_btnRename_clicked()
{
	QListWidgetItem *i = ui->lstPlaylists->currentItem();
	if(!i || !(i->flags() & Qt::ItemIsEditable))
		return;

	ui->lstPlaylists->editItem(i);
}

void PlaylistsDialog::on_lstPlaylists_currentItemChanged(QListWidgetItem *i, QListWidgetItem *)
{
	updateUiEnabled();

	if(!i) {
		itemsModel_.setStringList(QStringList());
		return;
	}

	// If the item is not editable, that means that the "current playlist" item is selected
	if(!(i->flags() & Qt::ItemIsEditable)) {
		QStringList items;

		for(const QSharedPointer<Presentation> &item : mainWindow->presentationMode()->playlist()->items())
			items << item->identification();

		itemsModel_.setStringList(items);
		return;
	}

	const qlonglong playlistId = i->data(Qt::UserRole).toLongLong();
	currentPlaylistJson_ = QJsonDocument::fromJson(db->selectValue("SELECT data FROM playlists WHERE id = ?", {playlistId}).toByteArray()).object();

	itemsModel_.setStringList(Playlist::itemNamesFromJSON(currentPlaylistJson_));
}

void PlaylistsDialog::on_btnLoad_clicked()
{
	auto playlist = mainWindow->presentationMode()->playlist();
	if(!playlist->items().isEmpty() && !standardConfirmDialog(tr("Aktuální program aplikace není prázdný. Načtením položky přepíšete. Opravdu chcete pokračovat?")))
		return;

	if(playlist->loadFromJSON(currentPlaylistJson_)) {
		accept();
		standardSuccessDialog(tr("Program načten."));
	}
}

void PlaylistsDialog::on_btnSave_clicked()
{
	if(!itemsModel_.stringList().isEmpty() && !standardConfirmDialog(tr("Opravdu chcete pokračovat a přepsat vybraný program?")))
		 return;

	QListWidgetItem *i = ui->lstPlaylists->currentItem();
	if(!i || !(i->flags() & Qt::ItemIsEditable))
		return;

	const qlonglong playlistId = i->data(Qt::UserRole).toLongLong();

	db->exec("UPDATE playlists SET data = ? WHERE id = ?", {QJsonDocument(mainWindow->presentationMode()->playlist()->toJSON()).toJson(QJsonDocument::Compact), playlistId});
	on_lstPlaylists_currentItemChanged(i, nullptr);

	accept();
	standardSuccessDialog(tr("Program uložen."));
}

void PlaylistsDialog::on_btnClose_clicked()
{
	accept();
}

void PlaylistsDialog::on_btnDelete_clicked()
{
	QListWidgetItem *i = ui->lstPlaylists->currentItem();
	if(!i || !(i->flags() & Qt::ItemIsEditable))
		return;

	if(!standardConfirmDialog(tr("Opravdu smazat vybraný program?")))
		return;

	db->exec("DELETE FROM playlists WHERE id = ?", {i->data(Qt::UserRole).toLongLong()});
	delete i;
}

void PlaylistsDialog::on_lstPlaylists_itemActivated(QListWidgetItem *)
{
	ui->btnLoad->click();
}
