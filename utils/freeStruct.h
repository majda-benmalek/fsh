#ifndef FREESTRUCT_H
#define FREESTRUCT_H

#include "commande.h"

// Déclaration des fonctions de libération de mémoire
void freeCmdSimple(cmd_simple *cmd);
void freeCmdStruct(commandeStruct *cmd);
void free_redirection(cmd_redirection *cmd);
void free_pipe(cmd_pipe *cmd);
void free_for(cmdFor *cmdFor);

#endif // FREESTRUCT_H