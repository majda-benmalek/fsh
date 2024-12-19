#ifndef PWD_H
#define PWD_H

#include <sys/types.h>
#include <dirent.h>

/**
 * Affiche le chemin absolu du répertoire courant.
 *
 * Cette fonction utilise `chemin_absolu` pour obtenir le chemin absolu du
 * répertoire courant et l'affiche sur la sortie standard.
 */
int pwd(void);

#endif // PWD_H
