#ifndef MULTISONGSELECTIONWIDGET_H
#define MULTISONGSELECTIONWIDGET_H

#include <QHash>
#include <QTreeWidgetItem>
#include <QWidget>

#include "modelview/songrecorditemmodel.h"

namespace Ui {
	class MultiSongSelectionWidget;
}

class MultiSongSelectionWidget : public QWidget {
	Q_OBJECT

public:
	explicit MultiSongSelectionWidget(QWidget *parent = 0);
	~MultiSongSelectionWidget();

public:
	void setDb(DBManager *mgr, bool allowEdit);

public:
	void setSelectedSongs(const QVector<qlonglong> &songIds);
	void setSelectedSongsIfReasonable(const QVector<qlonglong> &songIds);
	QVector<qlonglong> selectedSongs() const;
	bool isAnySongSelected() const;

public slots:
	void focusSongList();
	void clearSelection();

signals:
	void sigSelectionChanged();

private slots:
	void deleteSelected();

private slots:
	void onSelectionItemsManipulated(int index, int count);

private slots:
	void on_btnAddAll_clicked();
	void on_btnClear_clicked();
	void on_wgtSongList_sigItemActivated(const qlonglong songId);
	void on_tvSelection_activated(const QModelIndex &index);
	void on_btnRemoveAll_clicked();

private:
	Ui::MultiSongSelectionWidget *ui;
	SongRecordItemModel selectionModel_;
};

#endif// MULTISONGSELECTIONWIDGET_H
