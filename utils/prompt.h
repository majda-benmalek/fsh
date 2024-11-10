#ifndef PROMPT_H
#define PROMPT_H

#include <stddef.h>
#include <stdbool.h>

/**
 * Découpe une chaîne de caractères si elle dépasse une certaine longueur.
 * 
 * @param prompt La chaîne de caractères à découper.
 * @return Une nouvelle chaîne de caractères découpée si la longueur dépasse 30, sinon la chaîne originale.
 */
char *decoupe(char *prompt);

/**
 * Affiche l'invite de commande et récupère l'entrée de l'utilisateur.
 * 
 * @param chemin Le chemin du répertoire courant.
 * @param input Un pointeur vers une chaîne de caractères pour stocker l'entrée de l'utilisateur.
 */
void prompt(char *chemin, char *input,bool ok);

/**
 * Enlève le chemin du répertoire personnel de l'utilisateur du chemin du répertoire courant.
 * 
 * @param chemin Le chemin du répertoire courant.
 * @param home Le chemin du répertoire personnel de l'utilisateur.
 */
void enlever_home(char *chemin, const char *home);

#endif // PROMPT_H