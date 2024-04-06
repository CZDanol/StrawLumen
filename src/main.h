#ifndef MAIN_H
#define MAIN_H

#include <QDir>
#include <QString>

extern QDir appDataDirectory;
extern bool isPortableMode;

void initApplication();
void uninitApplication();

void setupStylesheet(bool darkMode);
void setupLanguage();

void criticalBootError(const QString &message);

#endif// MAIN_H
