#ifndef IF_H
#define IF_H

#include "gestionStruct.h"

/**
 * Exécute une commande conditionnelle en fonction du résultat d'un test.
 * @param cmd une structure cmdIf contenant le test à exécuter et les commandes à exécuter en fonction du résultat du test
 * @param chemin le chemin actuel du répertoire de travail
 * La fonction fork un processus pour exécuter le test. Si le test réussit (retourne 0), la commandeIf est exécutée.
 * Sinon, la commandeElse est exécutée. Les codes de retour des commandes sont récupérés et retournés.
 * @return le code de retour de la commande exécutée, ou 1 en cas d'erreur.
 */
int exec_if(cmdIf *cmd, char *chemin);

#endif // IF_H