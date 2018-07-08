#ifndef PRESENTATIONSTYLEPREVIEWWIDGET_H
#define PRESENTATIONSTYLEPREVIEWWIDGET_H

#include <QWidget>

#include "rec/presentationstyle.h"

class PresentationStylePreviewWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationStylePreviewWidget(QWidget *parent = nullptr);

public slots:
	void setPresentationStyle(const PresentationStyle &style);

protected:
	void paintEvent(QPaintEvent *event) override;

private slots:
	void onBackgroundManagerBackgroundLoaded(const QString &filename);

private:
	PresentationStyle presentationStyle_;

};

#endif // PRESENTATIONSTYLEPREVIEWWIDGET_H
