#ifndef PARSEBIBLE_H
#define PARSEBIBLE_H

#include <QString>

/// Checks if there are no bibles in the database and if there are bibles available for import
void checkBibleImport();

/// Forces the provided bibles to be imported
void forceImportBibles(const QStringList &filenames);

bool parseBible(const QString &filename);
void deleteBible(const QString &translationId);

#endif// PARSEBIBLE_H
