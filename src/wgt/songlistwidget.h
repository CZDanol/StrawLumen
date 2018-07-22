#ifndef SONGLISTWIDGET_H
#define SONGLISTWIDGET_H

#include <QWidget>
#include <QSqlQuery>
#include <QTimer>

#include "modelview/songsitemmodel.h"

namespace Ui {
	class SongListWidget;
}

class SongListWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SongListWidget(QWidget *parent = 0);
	~SongListWidget();

signals:
	/// Beware - first is songId (not row id), second is rowid in the view
	void sigCurrentChanged(qlonglong songId, int previousRowId);
	void sigSelectionChanged();
	void sigItemActivated(qlonglong songId);
	void sigCustomContextMenuRequested(const QPoint &globalPos);

public:
	int currentRowIndex() const;
	int selectedRowCount() const;
	qlonglong currentRowId() const;
	QVector<qlonglong> selectedRowIds() const;

	/// Returns vector of row ids that are currently in the song list (considering user filter)
	QVector<qlonglong> rowIds() const;

public slots:
	void unselect();
	void requery();
	void requeryTags();
	void requeryIfFilterChanged();

	void selectRow(int rowId);

	void clearFilters();

	void setDragEnabled(bool set);

protected:
	void showEvent(QShowEvent *e) override;

private slots:
	void onCurrentSongChanged(const QModelIndex &index, const QModelIndex &prevIndex);
	void onCurrentTagChanged(const QModelIndex &index, const QModelIndex &prevIndex);
	void onSongItemActivated(const QModelIndex &index);
	void onTextTypedToList(const QString &text);

private slots:
	void on_tvSongs_customContextMenuRequested(const QPoint &pos);
	void on_lnSearch_sigDownPressed();
	void on_tvSongs_sigUpPressed();
	void on_lvTags_activated(const QModelIndex &index);
	void on_btnClearTagFilter_clicked();

private:
	Ui::SongListWidget *ui;
	SongsItemModel songsModel_;
	QSqlQueryModel tagsModel_;
	QString currentFilterText_;
	QTimer typingTimer_;

};

#endif // SONGLISTWIDGET_H
