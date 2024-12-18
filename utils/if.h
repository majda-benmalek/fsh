#ifndef IF_H
#define IF_H

#include "gestionStruct.h"

/**
 * @brief Exécute une commande conditionnelle "if".
 *
 * @param cmd Structure contenant les informations de la commande "if".
 * @param chemin Chemin actuel du répertoire de travail.
 * @return Le code de retour de la commande exécutée.
 */
int exec_if(cmdIf *cmd, char *chemin);

#endif // IF_H