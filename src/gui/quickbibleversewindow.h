#pragma once

#include <QDialog>

#include "presentation/native/presentation_bibleverse.h"

namespace Ui {
	class QuickBibleVerseWindow;
}

class QuickBibleVerseWindow : public QDialog
{
	Q_OBJECT

public:
	explicit QuickBibleVerseWindow(QWidget *parent = nullptr);
	~QuickBibleVerseWindow();

	static QuickBibleVerseWindow *instance();

protected:
	virtual void showEvent(QShowEvent *e) override;
	virtual void closeEvent(QCloseEvent *e) override;
	virtual void hideEvent(QHideEvent *e) override;

private:
	void onHideOrClose();

private:
	Ui::QuickBibleVerseWindow *ui;
	QSharedPointer<Presentation_BibleVerse> presentation_;

private:
	struct {
		bool active;
		int slideId;
	} originalPresentationState_;

};

