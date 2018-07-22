#ifndef EXTENDEDTREEVIEW_H
#define EXTENDEDTREEVIEW_H

#include <QTreeView>

class ExtendedTreeView : public QTreeView
{
	Q_OBJECT

public:
	explicit ExtendedTreeView(QWidget *parent = nullptr);

signals:
	void sigUpPressed();
	void sigDownPressed();
	void sigTextTyped(const QString &text);

protected:
	virtual void keyPressEvent(QKeyEvent *e) override;

};

#endif // EXTENDEDTREEVIEW_H
