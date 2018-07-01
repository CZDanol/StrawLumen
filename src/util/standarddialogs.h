#ifndef STANDARDDIALOGS_H
#define STANDARDDIALOGS_H

#include "gui/mainwindow.h"

bool deleteConfirmDialog( const QString message, QWidget *parent = mainWindow );
void standardErrorDialog(const QString message, QWidget *parent = mainWindow );

bool standardConfirmDialog(const QString message, QWidget *parent = mainWindow );

#endif // STANDARDDIALOGS_H
