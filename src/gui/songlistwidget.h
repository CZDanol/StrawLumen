#ifndef SONGLISTWIDGET_H
#define SONGLISTWIDGET_H

#include <QWidget>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QTimer>

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
	void sigItemActivated();

public:
	int currentRowId();

public slots:
	void unselect();
	void requery();
	void requeryIfNecessary();

	void selectRow(int rowId);

protected:
	void showEvent(QShowEvent *e) override;

private slots:
	void onCurrentChanged(const QModelIndex &index, const QModelIndex &prevIndex);

private:
	Ui::SongListWidget *ui;
	QSqlQueryModel model_;
	QString currentFilterText_;
	QTimer typingTimer_;

};

#endif // SONGLISTWIDGET_H
