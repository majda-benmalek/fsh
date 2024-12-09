#ifndef GESTION_H
#define GESTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commande.h"

/**
 * @brief Analyse une commande utilisateur pour séparer la commande et ses arguments.
 *
 * Cette fonction prend une chaîne de caractères représentant une commande utilisateur
 * et la sépare en deux parties : la commande principale et ses arguments. La commande
 * et les arguments sont stockés dans les tampons `cmd` et `arg` respectivement.
 *
 * @param input La chaîne de caractères représentant la commande utilisateur.
 * @param arg Un pointeur vers un tampon de caractères où les arguments seront stockés.
 * @param cmd Un pointeur vers un tampon de caractères où la commande principale sera stockée.
 */
void gestion_cmd(char *input, commandeStruct *cmdstruct);

/**
 * @brief Exécute une commande shell en fonction de la commande et des arguments fournis.
 *
 * Cette fonction prend une commande et ses arguments, et exécute l'action appropriée
 * en fonction de la commande. Elle gère plusieurs commandes internes telles que `exit`,
 * `cd`, `pwd`, ainsi que des redirections et des boucles `for`. Si la commande n'est pas
 * reconnue, elle tente de l'exécuter comme une commande externe.
 *
 * @param cmd La commande à exécuter.
 * @param arg Les arguments de la commande.
 * @param input La ligne de commande complète entrée par l'utilisateur.
 * @param chemin Le chemin du répertoire courant.
 * @param dernier_exit Un pointeur vers un entier représentant la valeur de retour de la dernière commande exécutée.
 * @return La valeur de retour de la commande exécutée.
 */
int fsh(char *input, char *chemin, int *dernier_exit , commandeStruct *cmdstruct);

void make_cmdSimple(char *input, commandeStruct *cmdstruct);

#endif // GESTION_H