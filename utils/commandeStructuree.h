#ifndef COMMANDESTRUCTUREE_H
#define COMMANDESTRUCTUREE_H
#include "commande.h"
#include "gestion.h"



/**
 * @brief Trouve la fin d'un bloc de commandes en cherchant la fin d'un bloc de commandes délimité par des accolades `{}`
 * @param args Un tableau de chaînes de caractères représentant les arguments de la commande.
 * @param debut L'index à partir duquel  la recherche commence
 * @return  L'index de la fin du bloc dans le tableau `args`. Si aucun bloc n'est trouvé, la fonction retourne l'index du dernier argument.
 */

int trouver_fin_bloc(char** args , int debut);

/**
 * @brief Découpe une commande en arguments et remplit un tableau de commandeStruct.
 *
 * @param commande La commande à analyser.
 * @param cmds Tableau où ajouter la commande analysée.
 * @param nbCmds Pointeur vers le nombre actuel de commandes dans cmds.
 * @param maxCmds Nombre maximum de commandes autorisées dans cmds.
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
int decoupe_commande(char** commande , commandeStruct ** cmds , int * nbCmds , int maxCmds);

/**
 * @brief Analyse une chaîne d'entrée contenant plusieurs commandes structurées.
 *
 * @param args La chaîne d'entrée contenant les commandes à analyser.
 * @param cmds Tableau où ajouter les commandes analysées.
 * @param max_cmd Nombre maximum de commandes autorisées dans cmds.
 * @return Le nombre de commandes analysées en cas de succès, -1 en cas d'erreur.
 */

int decoupe_args(char** args , commandeStruct** cmds , int maxcmds);





#endif