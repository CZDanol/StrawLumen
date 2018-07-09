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
	void sigSelectionChanged(qlonglong songId, int previousRowId);
	void sigItemActivated(qlonglong songId);
	void sigCustomContextMenuRequested(const QPoint &globalPos);

public:
	int currentRowId();

public slots:
	void unselect();
	void requery();
	void requeryIfFilterChanged();

	void selectRow(int rowId);

	void setMultiSelectionEnabled(bool set);

protected:
	void showEvent(QShowEvent *e) override;

private slots:
	void onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex);
	void onCustomContextMenuRequested(const QPoint &pos);
	void onItemActivated(const QModelIndex &index);

private:
	Ui::SongListWidget *ui;
	SongsItemModel model_;
	QString currentFilterText_;
	QTimer typingTimer_;

};

#endif // SONGLISTWIDGET_H
