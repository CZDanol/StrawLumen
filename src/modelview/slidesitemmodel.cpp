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
		connect(playlist_.data(), SIGNAL(sigSlidesChanged()), this, SLOT(onSlidesChanged()));

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
	return 3;
}

QVariant SlidesItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal)
		return QVariant();

	if(role == Qt::DisplayRole) {
		switch(section) {

		case Column::presentation:
			return tr("Prezentace");

		case Column::id:
			return tr("Snímek");

		case Column::text:
			return tr("Text");

		default:
			return QVariant();

		}

	} else if(role == Qt::TextAlignmentRole)
		 return section == (int) Column::id ? Qt::AlignCenter : QVariant();

	else
		 return QVariant();
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
	auto slideId = index.row() - presentation->globalSlideIdOffset();

	const auto column = index.column();

	if (role == Qt::DisplayRole) {
		switch(column) {

		case Column::presentation:
			return presentation->identification();

		case Column::id:
			return presentation->slideIdentification(slideId);

		case Column::text:
			return presentation->slideDescription(slideId);

		default:
			return QVariant();

		}

	} else if(role == Qt::DecorationRole) {
		switch( column ) {

		case Column::presentation:
			return presentation->icon();

		default:
			return QVariant();

		}
	}

	else if(role == (int) UserData::isLastInPresentation)
		return slideId == presentation->slideCount() - 1;

	else if(role == (int) UserData::customIcon)
		return presentation->slideIdentificationIcon(slideId);

	else if(role == Qt::TextAlignmentRole)
		return column == (int) Column::id ? Qt::AlignCenter : QVariant();

	else
		return QVariant();
}

void SlidesItemModel::onSlidesChanged()
{
	beginResetModel();
	endResetModel();
	emit sigAfterSlidesChanged();
}
