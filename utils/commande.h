#ifndef COMMANDE_H
#define COMMANDE_H

typedef struct commandeStruct commandeStruct;

typedef enum
{
    CMD_EXTERNE,
    CMD_INTERNE,
    REDIRECTION,
    CMD_STRUCT,
    FOR,
    IF,
    PIPE
} Type;

typedef struct
{
    Type type;
    char **args;
} cmd_simple;

typedef struct
{
    Type type;
    cmd_simple **commandes;
    int nbCommandes;
} cmd_pipe;

typedef struct
{
    Type type;
    cmd_pipe *test;
    commandeStruct **commandeIf;
    commandeStruct **commandeElse;
} cmdIf;

typedef struct
{
    Type type;
    char *rep;
    char **op;
    char *variable;
    // int nbCommandes;
    commandeStruct **cmd;
} cmdFor;

typedef struct cmd_redirection cmd_redirection;

struct cmd_redirection
{
    Type type;
    cmd_simple *cmd;
    char *fichier;
    char *separateur;
} ;

struct commandeStruct
{
    Type type;
    cmd_simple *cmdSimple;
    cmd_pipe *pipe;
    cmdIf *cmdIf;
    cmdFor *cmdFor;
    cmd_redirection *cmdRed;
    commandeStruct ** cmdsStruc;
    int nbCommandes;
};

#endif