#ifndef PRESENTATIONPROPERTIESWIDGET_IMAGES_H
#define PRESENTATIONPROPERTIESWIDGET_IMAGES_H

#include <QModelIndexList>
#include <QWidget>

namespace Ui {
	class PresentationPropertiesWidget_Images;
}

class Presentation_Images;

class PresentationPropertiesWidget_Images : public QWidget {
	Q_OBJECT

public:
	explicit PresentationPropertiesWidget_Images(const QSharedPointer<Presentation_Images> &presentation, QWidget *parent = nullptr);
	virtual ~PresentationPropertiesWidget_Images() override;

protected:
	virtual bool eventFilter(QObject *obj, QEvent *ev) override;

private slots:
	void fillData();
	void deleteSelection();

private slots:
	void on_btnAddItems_clicked();
	void on_cbAutoPresentation_clicked(bool checked);
	void on_sbAutoInterval_valueChanged(int arg1);

	void on_lnName_editingFinished();

private:
	Ui::PresentationPropertiesWidget_Images *ui;
	QSharedPointer<Presentation_Images> presentation_;
};

#endif// PRESENTATIONPROPERTIESWIDGET_IMAGES_H
