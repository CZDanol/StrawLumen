#include "bibletranslationlistbox.h"

#include "job/db.h"
#include "job/settings.h"
#include "util/updatesdisabler.h"

BibleTranslationListBox::BibleTranslationListBox(QWidget *parent) : QListWidget(parent) {
	setDragDropMode(DragDropMode::InternalMove);
	setDefaultDropAction(Qt::MoveAction);
	setMinimumWidth(400);
	setMinimumHeight(200);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	connect(this, &QListWidget::itemChanged, this, &BibleTranslationListBox::sigChanged);
	connect(model(), &QAbstractItemModel::rowsMoved, this, &BibleTranslationListBox::sigChanged);
}

void BibleTranslationListBox::updateList() {
	QStringList selectedTranslations;
	for(qsizetype i = 0; i < count(); i++) {
		QListWidgetItem *wi = item(i);
		if(wi->checkState() == Qt::Checked)
			selectedTranslations += wi->data(Qt::UserRole).toString();
	}

	updateList(selectedTranslations);
}

void BibleTranslationListBox::updateList(const QStringList &selection) {
	UpdatesDisabler _ud(this);
	clear();

	QMap<QString, QListWidgetItem *> items;

	QSqlQuery q = db->selectQuery("SELECT translation_id, name FROM bible_translations ORDER BY translation_id");
	while(q.next()) {
		const QString trId = q.value(0).toString();
		QListWidgetItem *wi = new QListWidgetItem(trId + " | " + q.value(1).toString());
		wi->setData(Qt::UserRole, trId);
		wi->setCheckState(Qt::Unchecked);
		items[trId] = wi;
	}

	// Add selected translations first, in the order they were selected
	for(const QString &tr: std::as_const(selection)) {
		QListWidgetItem *wi = items.take(tr);
		if(!wi)
			continue;

		wi->setCheckState(Qt::Checked);
		addItem(wi);
	}

	// The add the unselected items in alphabetical order
	for(QListWidgetItem *wi: std::as_const(items))
		addItem(wi);
}

QStringList BibleTranslationListBox::selectedTranslations() const {
	QStringList lst;

	for(qsizetype i = 0; i < count(); i++) {
		const QListWidgetItem *wi = item(i);
		if(wi->checkState() == Qt::Checked)
			lst += wi->data(Qt::UserRole).toString();
	}

	return lst;
}
