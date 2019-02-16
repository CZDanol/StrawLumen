#ifndef PRESENTATIONPROPERTIESWIDGET_WEB_H
#define PRESENTATIONPROPERTIESWIDGET_WEB_H

#include <QWidget>

namespace Ui {
	class PresentationPropertiesWidget_Web;
}

class Presentation_Web;

class PresentationPropertiesWidget_Web : public QWidget
{
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_Web(const QSharedPointer<Presentation_Web> &presentation, QWidget *parent = nullptr);
	~PresentationPropertiesWidget_Web();

private slots:
	void fillData();
	void on_lnUrl_editingFinished();

	void on_btnLocalFile_clicked();

private:
	Ui::PresentationPropertiesWidget_Web *ui;
	QSharedPointer<Presentation_Web> presentation_;

};

#endif // PRESENTATIONPROPERTIESWIDGET_WEB_H
