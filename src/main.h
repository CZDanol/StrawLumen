#ifndef MAIN_H
#define MAIN_H

#include <QDir>
#include <QString>

extern QDir appDataDirectory;

void initApplication();
void uninitApplication();

void setupStylesheet(bool darkMode);

void criticalBootError(const QString &message);

#endif // MAIN_H
