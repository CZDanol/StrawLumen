#ifndef PRESENTATIONPROPERTIESWIDGET_POWERPOINT_H
#define PRESENTATIONPROPERTIESWIDGET_POWERPOINT_H

#include <QWidget>
#include <QSharedPointer>

class Presentation_PowerPoint;

namespace Ui {
	class PresentationPropertiesWidget_PowerPoint;
}

class PresentationPropertiesWidget_PowerPoint : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_PowerPoint(const QSharedPointer<Presentation_PowerPoint> &presentation, QWidget *parent);
	~PresentationPropertiesWidget_PowerPoint();

private:
	void updateUiEnabled();

private slots:
	void on_cbAutoPresentation_clicked(bool checked);
	void on_cbBlackSlideBefore_clicked(bool checked);
	void on_cbBlackSlideAfter_clicked(bool checked);

private:
	Ui::PresentationPropertiesWidget_PowerPoint *ui;
	QSharedPointer<Presentation_PowerPoint> presentation_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_POWERPOINT_H
