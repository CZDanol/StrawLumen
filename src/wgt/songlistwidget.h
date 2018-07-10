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
	int currentRowId() const;
	int selectedRowCount() const;
	QVector<qlonglong> selectedRowIds() const;

public slots:
	void unselect();
	void requery();
	void requeryIfFilterChanged();

	void selectRow(int rowId);

	void setDragEnabled(bool set);

protected:
	void showEvent(QShowEvent *e) override;

private slots:
	void onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex);
	void onItemActivated(const QModelIndex &index);

private slots:
	void on_tvList_customContextMenuRequested(const QPoint &pos);

private:
	Ui::SongListWidget *ui;
	SongsItemModel model_;
	QString currentFilterText_;
	QTimer typingTimer_;

};

#endif // SONGLISTWIDGET_H
