#ifndef GESTIONSTRUCT_H
#define GESTIONSTRUCT_H

#include <stdbool.h>
#include "commande.h"

int arg_cmdsimple(char **args, char **commande, int i, int j);

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

commandeStruct *remplissage_cmdStruct(Type type, cmd_simple *cmdSimple, cmd_pipe *pipestruct, cmdIf *cmdIfStruct, cmdFor *cmdForStruct, commandeStruct **cmdsStrcut, int nbcommandes, commandeStruct *cmd);

/**
 * @brief Alloue et remplit une structure cmd_simple à partir d'une liste d'arguments.
 *
 * @param args Tableau de chaînes représentant les arguments de la commande.
 * @return Pointeur vers la structure cmd_simple allouée, ou NULL en cas d'échec.
 */
cmd_simple *remplissage_cmdSimple(char **args);

/**
 * Remplit une structure cmd_redirection à partir d'un tableau d'arguments.
 * @param args un tableau de chaînes de caractères représentant les arguments de la commande avec redirection
 * La fonction analyse les arguments pour extraire la commande à exécuter et le fichier de redirection.
 * Elle gère différents types de redirections comme ">", ">>", "2>", etc.
 * @return une structure cmd_redirection remplie, ou NULL en cas d'erreur de syntaxe ou d'allocation de mémoire.
 */
cmd_redirection *remplissageCmdRedirection(char **args);

/**
 * Remplit une structure cmd_pipe à partir d'un tableau d'arguments.
 * @param args un tableau de chaînes de caractères représentant les arguments de la commande avec pipe
 * La fonction analyse les arguments pour extraire les commandes à exécuter et les sépare par des pipes.
 * Elle gère les erreurs de syntaxe et d'allocation de mémoire.
 * @return une structure cmd_pipe remplie, ou NULL en cas d'erreur de syntaxe ou d'allocation de mémoire.
 */
cmd_pipe *remplissageCmdPipe(char **args);

/**
 * @brief Remplit une structure `commandeStruct` 
 * @param args un tableau contenant les arguments des commandes
 * @param cmdStruct Un pointeur vers une `commandeStruct` à remplir
 */
void remplissageCmdStructurees(char **args, commandeStruct *cmdStruct);

size_t tailleArgs(char **args);

cmdFor *make_for(char **args);

/**
 * Remplit une structure cmdIf à partir d'un tableau d'arguments.
 * @param args un tableau de chaînes de caractères représentant les arguments de la commande if
 * La fonction analyse les arguments pour extraire le test, la commande à exécuter si le test réussit (commandeIf),
 * et la commande à exécuter si le test échoue (commandeElse). Elle gère également les blocs imbriqués.
 * @return une structure cmdIf remplie, ou NULL en cas d'erreur de syntaxe ou d'allocation de mémoire.
 */
cmdIf *remplissageCmdIf(char **args);

#endif