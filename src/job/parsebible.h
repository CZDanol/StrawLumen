#ifndef PARSEBIBLE_H
#define PARSEBIBLE_H

#include <QString>

/// Checks if there are no bibles in the database and if there are bibles available for import
void checkBibleImport();

void parseBible(const QString &filename);

#endif // PARSEBIBLE_H
