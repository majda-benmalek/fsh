#ifndef COMMANDESTRUCTUREE_H
#define COMMANDESTRUCTUREE_H
#include "commande.h"
#include "gestion.h"

/**
 * @brief Découpe une commande en arguments et remplit un tableau de commandeStruct.
 *
 * @param commande La commande à analyser.
 * @param cmds Tableau où ajouter la commande analysée.
 * @param nbCmds Pointeur vers le nombre actuel de commandes dans cmds.
 * @param maxCmds Nombre maximum de commandes autorisées dans cmds.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int decoupe_commande(char* commande , commandeStruct ** cmds , int * nbCmds , int maxCmds);

/**
 * @brief Analyse une chaîne d'entrée contenant plusieurs commandes structurées.
 *
 * @param input La chaîne d'entrée contenant les commandes à analyser.
 * @param cmds Tableau où ajouter les commandes analysées.
 * @param max_cmd Nombre maximum de commandes autorisées dans cmds.
 * @return Le nombre de commandes analysées en cas de succès, -1 en cas d'erreur.
 */

int gestion_commandes_structures(char* input , commandeStruct** cmds , int max_cmd);





#endif