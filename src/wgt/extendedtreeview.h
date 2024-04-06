#ifndef EXTENDEDTREEVIEW_H
#define EXTENDEDTREEVIEW_H

#include <QTreeView>

class ExtendedTreeView : public QTreeView {
	Q_OBJECT

public:
	using StartDragFunction = std::function<bool(Qt::DropActions)>;

public:
	explicit ExtendedTreeView(QWidget *parent = nullptr);

public:
	void setStartDragFunction(const StartDragFunction &f);

signals:
	void sigUpPressed();
	void sigDownPressed();
	void sigTextTyped(const QString &text);

protected:
	virtual void keyPressEvent(QKeyEvent *e) override;
	virtual void startDrag(Qt::DropActions supportedActions) override;

private:
	StartDragFunction startDragFunction_;
};

#endif// EXTENDEDTREEVIEW_H
