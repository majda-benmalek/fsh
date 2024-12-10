#ifndef GESTIONSTRUCT_H
#define GESTIONSTRUCT_H

#include <stdbool.h>
#include "commande.h"

/** 
* @brief Alloue et remplit une structure commandeStruct. 
* 
* @param type Type de commande (interne, externe, etc.). 
* @param cmdSimple Pointeur vers une commande simple. 
* @param pipestruct Pointeur vers une structure de pipe. 
* @param cmdIfStruct Pointeur vers une structure de commande conditionnelle. 
* @param cmdForStruct Pointeur vers une structure de commande For. 
* @param nbcommandes Nombre de commandes dans la structure. 
* @return Pointeur vers la structure commandeStruct allouée, ou NULL en cas d'échec. */


commandeStruct *remplissage_cmdStruct(Type type , cmd_simple * cmdSimple , cmd_pipe* pipestruct , cmdIf* cmdIfStruct , cmdFor * cmdForStruct , int nbcommandes ) ;

/**
 * @brief Libère la mémoire allouée pour une structure cmd_simple.
 * 
 * @param cmd Pointeur vers la structure cmd_simple à libérer.
*/

void freeCmdSimple(cmd_simple* cmd);

/**
 * @brief Libère la mémoire allouée pour une structure pipe.
 * 
 * @param cmd Pointeur vers la structure pipe à libérer.
*/

void freePipe(cmd_pipe* pipeStruct);


/**
 * @brief Libère la mémoire allouée pour une structure commandeStruct et ses sous-structures.
 * 
 * @param cmd Pointeur vers la structure commandeStruct à libérer.
*/

void freeCmdStruct(commandeStruct* cmd);

/**
 * @brief Alloue et remplit une structure cmd_simple à partir d'une liste d'arguments. 
 * 
 * @param args Tableau de chaînes représentant les arguments de la commande. 
 * @return Pointeur vers la structure cmd_simple allouée, ou NULL en cas d'échec. 
*/
cmd_simple *remplissage_cmdSimple(char ** args );

#endif