#ifndef CMDSTRUCTUREE_H
#define CMDSTRUCTUREE_H
#include "commande.h"
#include "gestion.h"
#include "commandeStructuree.h"

/**
 * @brief Exécute une série de commandes structurées.
 * @param cmds Un tableau de pointeurs vers des structures `commandeStruct` représentant les commandes à éxecuter
 * @param nbCommandes le nombre de commandes dans le tableau cmds
 * @param chemin Le chemin à passer à la fonction `fsh` lors de l'exécution des commandes.
 * @return La valeur de retour de la derniére commande exécutée, -1 en cas d'erreur.
 */
int execCmdStruct(commandeStruct **cmds, int nbCommandes ,char * chemin);

#endif // CMDSTRUCTUREE_H