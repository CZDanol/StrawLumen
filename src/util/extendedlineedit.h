#ifndef EXTENDEDLINEEDIT_H
#define EXTENDEDLINEEDIT_H

#include <QLineEdit>

class ExtendedLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	explicit ExtendedLineEdit(QWidget *parent = nullptr);

signals:
	void sigFocused();

protected:
	virtual void focusInEvent(QFocusEvent *e) override;

};

#endif // EXTENDEDLINEEDIT_H
