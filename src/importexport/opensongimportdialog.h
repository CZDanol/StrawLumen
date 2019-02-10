#ifndef OPENSONGIMPORTDIALOG_H
#define OPENSONGIMPORTDIALOG_H

#include <QDialog>

#include "presentation/presentation.h"

namespace Ui {
	class OpenSongImportDialog;
}

class OpenSongImportDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OpenSongImportDialog(QWidget *parent = nullptr);
	~OpenSongImportDialog();

public:
	void show();
	void show(const QStringList &files);

private:
	void updateUi();

private:
	QString importSong(const QString &filename, const int conflictBehavior, const QSet<QString> &tags, const bool addToPlaylist, QVector<QSharedPointer<Presentation> > &presentations);

private slots:
	void on_btnClose_clicked();
	void on_btnImport_clicked();
	void on_btnSelectFiles_clicked();
	void on_btnFolderStructure_clicked();
	void on_optSingleSongs_clicked();
	void on_optDirectoryTree_clicked();

private:
	Ui::OpenSongImportDialog *ui;
	QStringList importFiles_;
	QString importDirectory_;

};

OpenSongImportDialog *openSongImportDialog();

#endif // OPENSONGIMPORTDIALOG_H
