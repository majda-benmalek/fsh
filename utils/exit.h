#ifndef EXIT_H
#define EXIT_H
#include <stdlib.h>

extern int dernier_exit ; 
/**
* Termine le processus fsh avec comme valeur de retour *'valeur' a été donnée en paramétre sinon la valeur de *retour de la dernière commande exécutée.

*@param valeur Chaîne de caractères qui représente le code se sortie si NULL le programme quitte avec 'dernier_exit'
 */
void commande_exit( char * valeur);

#endif