#ifndef PARSEBIBLE_H
#define PARSEBIBLE_H

#include <QString>

/// Checks if there are no bibles in the database and if there are bibles available for import
void checkBibleImport();

bool parseBible(const QString &filename);
void deleteBible(const QString &translationId);

#endif // PARSEBIBLE_H
