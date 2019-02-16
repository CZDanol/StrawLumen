#ifndef PRESENTATIONPROPERTIESWIDGET_VIDEO_H
#define PRESENTATIONPROPERTIESWIDGET_VIDEO_H

#include <QWidget>

namespace Ui {
	class PresentationPropertiesWidget_Video;
}

class Presentation_Video;

class PresentationPropertiesWidget_Video : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_Video(const QSharedPointer<Presentation_Video> &presentation, QWidget *parent = nullptr);
	~PresentationPropertiesWidget_Video();

private slots:
	void fillData();
	void on_cbAutoPlay_clicked(bool checked);
	void on_cbRepeat_clicked(bool checked);

private:
	Ui::PresentationPropertiesWidget_Video *ui;
	QSharedPointer<Presentation_Video> presentation_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_VIDEO_H
