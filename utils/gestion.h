#ifndef GESTION_H
#define GESTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commande.h"

/**
 * @brief Analyse et remplit une structure commandeStruct en fonction des arguments fournis.
 *
 * @param args un tableau de chaînes de caractères représentant les arguments de la commande
 * @param cmdstruct une structure commandeStruct à remplir
 * La fonction détermine le type de commande (if, for, pipe, redirection, etc.) et remplit la structure cmdstruct en conséquence.
 * Elle gère également les erreurs de syntaxe et d'allocation de mémoire.
 */
void gestion_cmd(char **input, commandeStruct *cmdstruct);

/**
 * Exécute une commande en fonction de sa structure et de son type.
 * @param chemin le chemin actuel du répertoire de travail
 * @param dernier_exit un pointeur vers le code de retour de la dernière commande exécutée
 * @param cmdstruct une structure commandeStruct représentant la commande à exécuter
 * La fonction détermine le type de commande (for, interne, pipe, externe, redirection, struct, if) et exécute la commande en conséquence.
 * Elle gère également les erreurs de syntaxe et d'allocation de mémoire.
 * @return le code de retour de la commande exécutée, ou une valeur d'erreur spécifique.
 */
int fsh(char *chemin, int *dernier_exit, commandeStruct *cmdstruct);

int rechercheDansArgs(char *tofind, char **args);

#endif // GESTION_H