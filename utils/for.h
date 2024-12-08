#ifndef FOR_H
#define FOR_H


/**
 * @param input : une chaîne de caractére sous la forme  "for f in rep { cmd ; cmd ; ... }"
 * parcourt le repertoire rep et execute les commandes cmd pour chaque fichier du répertoir
 * @return 0 en cas de succès, 1 en cas d'erreur
 */

int boucle_for(char* input);

#endif