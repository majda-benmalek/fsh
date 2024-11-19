#ifndef PROMPT_H
#define PROMPT_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Affiche l'invite de commande et récupère l'entrée de l'utilisateur.
 * 
 * @param chemin Le chemin du répertoire courant.
 * @param input Un pointeur vers une chaîne de caractères pour stocker l'entrée de l'utilisateur.
 */
int prompt(char *chemin, char *input,int *ok);

#endif // PROMPT_H