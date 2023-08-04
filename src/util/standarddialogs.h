#ifndef STANDARDDIALOGS_H
#define STANDARDDIALOGS_H

#include <QWidget>

bool standardDeleteConfirmDialog( const QString message, QWidget *parent = nullptr);
bool standardConfirmDialog(const QString message, QWidget *parent = nullptr);

void standardErrorDialog(const QString message, QWidget *parent = nullptr, bool blocking = false);
void standardInfoDialog(const QString message, QWidget *parent = nullptr);
void standardSuccessDialog(const QString message, QWidget *parent = nullptr);

#endif // STANDARDDIALOGS_H
