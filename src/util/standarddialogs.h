#ifndef STANDARDDIALOGS_H
#define STANDARDDIALOGS_H

#include "gui/mainwindow.h"

bool standardDeleteConfirmDialog( const QString message, QWidget *parent = mainWindow);
bool standardConfirmDialog(const QString message, QWidget *parent = mainWindow);

void standardErrorDialog(const QString message, QWidget *parent = mainWindow, bool blocking = false);
void standardInfoDialog(const QString message, QWidget *parent = mainWindow);
void standardSuccessDialog(const QString message, QWidget *parent = mainWindow);

#endif // STANDARDDIALOGS_H
