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
	void sigDownPressed();
	void sigUpPressed();

protected:
	virtual void focusInEvent(QFocusEvent *e) override;
	virtual void keyPressEvent(QKeyEvent *e) override;

};

#endif // EXTENDEDLINEEDIT_H
