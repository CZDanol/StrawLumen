#ifndef PRESENTATIONPROPERTIESWIDGET_H
#define PRESENTATIONPROPERTIESWIDGET_H

#include <QWidget>
#include <QSharedPointer>

namespace Ui {
	class PresentationPropertiesWidget;
}

class Presentation;

class PresentationPropertiesWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget(const QSharedPointer<Presentation> &presentation, QWidget *parent = 0);
	~PresentationPropertiesWidget();

public:
	QSharedPointer<Presentation> presentation() const;

private:
	Ui::PresentationPropertiesWidget *ui;
	QSharedPointer<Presentation> presentation_;
	QWidget *propertiesWidget_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_H
