#ifndef PROMPT_H
#define PROMPT_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Compte le nombre de chiffres dans un nombre entier.
 *
 * Cette fonction prend un nombre entier en entrée et retourne le nombre de chiffres
 * qu'il contient. Si le nombre est négatif, le signe négatif n'est pas compté.
 *
 * @param nombre Le nombre entier dont on veut compter les chiffres.
 * @return Le nombre de chiffres dans le nombre.
 */
int compter_chiffres(int nombre);

/**
 * @brief Tronque un chemin pour qu'il tienne dans une longueur maximale.
 *
 * Cette fonction prend un chemin de fichier en entrée et le tronque si sa longueur dépasse
 * 25 caractères. Les premiers caractères du chemin tronqué sont remplacés par '...'.
 *
 * @param chemin Le chemin à tronquer.
 * @param ret Un pointeur vers un entier utilisé pour calculer la longueur du chemin tronqué.
 * @return Un pointeur vers la nouvelle chaîne de caractères contenant le chemin tronqué.
 *         La mémoire allouée pour cette chaîne doit être libérée par l'appelant.
 */
char *decoupe(char *chemin, int *ret);

/**
 * Affiche une invite de commande personnalisée et lit l'entrée de l'utilisateur.
 * @param chemin le chemin actuel du répertoire de travail
 * @param input un buffer pour stocker l'entrée de l'utilisateur
 * @param ret un pointeur vers un entier qui stocke le code de retour de la dernière commande exécutée
 * La fonction affiche une invite de commande colorée en fonction du code de retour de la dernière commande.
 * Elle lit ensuite l'entrée de l'utilisateur, la stocke dans le buffer input et ajoute cette entrée à l'historique des commandes.
 * @return 0 si l'entrée est lue avec succès, 1 sinon.
 */
int prompt(char *chemin, char *input,int *ret);

#endif // PROMPT_H