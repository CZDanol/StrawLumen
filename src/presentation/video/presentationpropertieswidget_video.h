#ifndef PRESENTATIONPROPERTIESWIDGET_VIDEO_H
#define PRESENTATIONPROPERTIESWIDGET_VIDEO_H

#include <QWidget>

namespace Ui {
	class PresentationPropertiesWidget_Video;
}

class PresentationPropertiesWidget_Video : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_Video(QWidget *parent = nullptr);
	~PresentationPropertiesWidget_Video();

private:
	Ui::PresentationPropertiesWidget_Video *ui;
};

#endif // PRESENTATIONPROPERTIESWIDGET_VIDEO_H
