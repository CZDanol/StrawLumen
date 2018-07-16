#ifndef BACKGROUNDDIALOG_H
#define BACKGROUNDDIALOG_H

#include <QDialog>
#include <QDir>
#include <QMenu>
#include <QListWidgetItem>
#include <QHash>

#include "rec/presentationbackground.h"

#define BACKGROUND_THUMBNAIL_SIZE 128

namespace Ui {
	class BackgroundDialog;
}

class BackgroundDialog : public QDialog
{
	Q_OBJECT

public:
	explicit BackgroundDialog(QWidget *parent = 0);
	~BackgroundDialog();

public:
	void showInMgmtMode();
	bool showInSelectionMode(PresentationBackground &background);

	const QDir &backgroundsDirectory() const;

protected:
	void dragEnterEvent(QDragEnterEvent *e) override;
	void dropEvent(QDropEvent *e) override;
	void showEvent(QShowEvent *e) override;

private:
	void loadBackgrounds(bool force = false);
	bool addBackground(const QString &filename);
	void setMgmtMode(bool set);

private slots:
	void updatePreview();

private slots:
	void onLwGalleryContextMenuRequested(const QPoint &pos);

private slots:
	void on_btnAdd_clicked();
	void on_lwList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
	void on_actionDelete_triggered();
	void on_wgtColor_sigColorChangedByUser(const QColor &);
	void on_lwList_itemActivated(QListWidgetItem *item);
	void on_cmbBlendMode_activated(int index);
	void on_btnResetColor_clicked();

private:
	Ui::BackgroundDialog *ui;
	QMenu *galleryContextMenu_;

private:
	bool isMgmtMode_;
	PresentationBackground presentationBackground_;

private:
	bool backgroundsLoaded_ = false;
	QDir backgroundsDirectory_;
	QHash<QString,QListWidgetItem*> itemsByFilename_;

};

extern BackgroundDialog *backgroundDialog;

#endif // BACKGROUNDDIALOG_H
