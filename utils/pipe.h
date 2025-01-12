#ifndef PIPE_H
#define PIPE_H
#include "commande.h"

/**
 * Exécute une série de commandes en utilisant des pipes pour rediriger la sortie de chaque commande vers l'entrée de la suivante.
 * @param p une structure cmd_pipe contenant les commandes à exécuter et le nombre de commandes
 * La fonction crée un pipe pour chaque paire de commandes, puis fork un processus pour chaque commande.
 * @return le code de retour de la dernière commande exécutée, ou 1 en cas d'erreur.
 */
int cmdpipe(cmd_pipe *p);

#endif