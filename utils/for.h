#ifndef FOR_H
#define FOR_H

extern int max;
/**
 * @param input : une chaîne de caractére sous la forme  "for f in rep { cmd ; cmd ; ... }"
 * parcourt le repertoire rep et execute les commandes cmd pour chaque fichier du répertoir
 * @return 0 en cas de succès, 1 en cas d'erreur
 */

// int make_for(char *input, cmdFor *cmdFor);

// int boucle_for(char* input);
int nouveau(char *ancienne, char *nouveau, commandeStruct *cmd);
int boucle_for(cmdFor *cmdFor);
#endif