#ifndef FONTSELECTIONWIDGET_H
#define FONTSELECTIONWIDGET_H

#include <QWidget>
#include <QStringListModel>

namespace Ui {
	class FontSelectionWidget;
}

class FontSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FontSelectionWidget(QWidget *parent = 0);
	~FontSelectionWidget();

signals:
	void sigFontChangedByUser(const QFont &newFont);

public:
	const QFont &selectedFont() const;

public slots:
	void updateFontList();
	void setSelectedFont(const QFont &font);
	void setReadOnly(bool set);

private slots:
	void updateFontStyleList(const QString &setStyle);

private slots:
	void on_cmbFont_activated(const QString &arg1);
	void on_cmbFontStyle_activated(const QString &arg1);
	void on_sbSize_valueChanged(int arg1);
	void on_btnBold_toggled(bool checked);
	void on_btnItalic_toggled(bool checked);

private:
	Ui::FontSelectionWidget *ui;
	bool isReadOnly_ = false;
	QFont selectedFont_;
	QStringListModel fontListModel_;
	QStringListModel fontStyleListModel_;

};

#endif // FONTSELECTIONWIDGET_H
