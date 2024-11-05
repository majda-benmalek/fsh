#ifndef CD_H
#define CD_H

#include <limits.h>

/**
 * Change le répertoire courant.
 * 
 * @param chemin Le chemin vers lequel changer le répertoire courant. Si NULL, change vers le répertoire HOME.
 *               Si "-", change vers le répertoire précédent.
 * @return 0 en cas de succès, 1 en cas d'erreur.
 */
int cd_commande(char *chemin);

#endif // CD_H