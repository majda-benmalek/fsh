#ifndef FOR_H
#define FOR_H

#include "dirent.h"

extern int max;
extern int retF;

/** 
 * compte le nombre d'occurence d'une @param sous_chaine  dans une @param chaine
 * @return le nombre d'occurence
*/
int compte_occ(char *chaine, char *sous_chaine);

/**
 * supprime le dernier / d'un path
 * @param path dont on veut supprimer le denrnier /
 */
void eleverSlash(char *path);

/** remplace la variable par le nom du fichier ou du dossier et inversement d'une commande simple
 * @param ancienne la chaîne à remplacer
 * @param nouveau la nouvelle chaine
 * @param cmd  la commande dans laquelle on veut remplacer
 * @return 0 en cas de succès 1 en cas d'erreur
 */
int nouveau_var_simple(char *ancienne, char *nouveau, cmd_simple *cmd);

/** remplace la variable par le nom du fichier ou du dossier et inversement d'une commande structurée
 * @param ancienne la chaîne à remplacer
 * @param nouveau la nouvelle chaine
 * @param cmd  la commande dans laquelle on veut remplacer
 * @return 0 en cas de succès 1 en cas d'erreur
 */
int nouveau_var(char *ancienne, char *nouveau, commandeStruct *cmd);

/** option du for  étend la portée de la boucle aux fichiers cachés (sauf . et ..); 
 * @param entry repertoire sur lequel on veut étendre la portée
 * @param cmdFor commande for à éxecuter
*/
int optionA(struct dirent *entry, cmdFor *cmdFor);

/**
 * renvoie l' argument d'une option
 * @param op l'option dont on veut son argument
 * @param for_opt la tableau des options du for
 * @return l'indice de l'argument
 */
int arg_options(char **op, char *for_opt);

/** option du for  filtrage des résultats selon l'extension 
 * @param entry le répertoire sur lequel on veut faire le filtrage
 * @param cmdFor la commande for à éxécuter
 * @return 0 succès  1 erreur
*/
int option_e(struct dirent *entry, cmdFor *cmdFor);


/** option du for : filtrage des résultats selon le type de fichier 
 * @param entry fichier
 * @param cmd commande for
 * @return  0 en cas de succès, -1 en cas d'erreur
 */
int option_t(struct dirent *entry, cmdFor *cmd);

/**
 * @param input : une chaîne de caractére sous la forme  "for f in rep { cmd ; cmd ; ... }"
 * parcourt le repertoire rep et execute les commandes cmd pour chaque fichier du répertoire
 * @return 0 en cas de succès, 1 en cas d'erreur
 */
int boucle_for(cmdFor *cmdFor);

/**
 * option du for : parcours récursif de toute l'arborescence de racine @param entry
 * @param cmd la commande for à executer
 * @return 0 en cas de succès, 1 en cas d'erreur
*/
int option_r(struct dirent *entry, cmdFor *cmd);


/** option du for : traitement en parallèle d'un maximum de MAX tours de boucle.
 * @param cmd la commande à l'intérieur du for à éxecuter
 * @param maxp le nombre de processus max qu'on peut créer
 *  @return  -7 pour le processus parant la valeur de retour de fsh pour les enfants
 */
int option_p(commandeStruct *cmd, int maxp);

#endif