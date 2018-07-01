#include "slidesitemmodel.h"

#include "presentation/playlist.h"
#include "presentation/presentation.h"

SlidesItemModel::SlidesItemModel()
{

}

void SlidesItemModel::setPlaylist(const QSharedPointer<Playlist> &playlist)
{
	if(playlist_ == playlist)
		return;

	beginResetModel();

	// Disconnect all signals from the previous playlist
	if(playlist_)
		playlist_->disconnect(this);

	playlist_ = playlist;

	if(playlist_)
		connect(playlist_.data(), SIGNAL(sigSlidesChanged()), this, SLOT(resetModel()));

	endResetModel();
}

int SlidesItemModel::rowCount(const QModelIndex &) const
{
	if(playlist_.isNull())
		return 0;

	return playlist_->slideCount();
}

int SlidesItemModel::columnCount(const QModelIndex &) const
{
	return 2;
}

QVariant SlidesItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::DisplayRole)
		return QVariant();

	if(orientation != Qt::Horizontal)
		return QVariant();

	switch(section) {

	case 0:
		return tr("Prezentace");

	case 1:
		return tr("Text");

	default:
		return QVariant();

	}
}

QVariant SlidesItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if(playlist_.isNull())
		return QVariant();

	if (index.row() >= playlist_->slideCount() || index.row() < 0)
		return QVariant();

	auto presentation = playlist_->presentationOfSlide(index.row());
	auto slideId = index.row() - presentation->firstSlideOffsetInPlaylist();

	const auto column = index.column();

	if (role == Qt::DisplayRole) {
		switch(column) {

		case 0:
			return presentation->identification();

		case 1:
			return presentation->slideDescription(slideId);

		default:
			return QVariant();

		}

	} else if(role == Qt::DecorationRole) {
		return column == 0 ? presentation->icon() : QVariant();

	}

	return QVariant();
}

void SlidesItemModel::resetModel()
{
	beginResetModel();
	endResetModel();
}
