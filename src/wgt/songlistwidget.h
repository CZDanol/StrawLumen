#ifndef SONGLISTWIDGET_H
#define SONGLISTWIDGET_H

#include <QWidget>
#include <QSqlQuery>
#include <QTimer>

#include "modelview/songsitemmodel.h"
#include "job/db.h"

namespace Ui {
	class SongListWidget;
}

class SongListWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SongListWidget(QWidget *parent = 0);
	~SongListWidget();

public:
	inline void setShowTags(bool set) {
		showTags_ = set;
	}

	void setDb(DBManager *mgr, bool allowDbEdit);

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

	QString currentTagFilterName() const;

	/// Returns vector of row ids that are currently in the song list (considering user filter)
	QVector<qlonglong> rowIds() const;

public slots:
	void focusSongList();

	void unselect();
	void requery();
	void requeryTags();
	void requeryIfFilterChanged();

	void selectRow(int rowId);

	void clearFilters();

	void setDragEnabled(bool set);

protected:
	void showEvent(QShowEvent *e) override;
	void keyPressEvent(QKeyEvent *e) override;

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
	void on_btnSearchInText_clicked();
	void on_lvTags_customContextMenuRequested(const QPoint &pos);
	void on_actionDeleteTag_triggered();

private:
	bool showTags_ = true;
	Ui::SongListWidget *ui;
	SongsItemModel songsModel_;
	QSqlQueryModel tagsModel_;
	QString currentFilterText_;
	QString currentTagFitler();
	QTimer typingTimer_;

private:
	DBManager *db_ = nullptr;
	bool allowDbEdit_ = false;

};

#endif // SONGLISTWIDGET_H
