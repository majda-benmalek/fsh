#ifndef COMMANDE_H
#define COMMANDE_H

typedef struct commandeStruct commandeStruct;
typedef struct cmd_redirection cmd_redirection;
typedef struct cmd_redirection cmd_redirection;

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
    cmd_redirection *red;
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
    commandeStruct *test;
    commandeStruct *commandeIf;
    commandeStruct *commandeElse;
} cmdIf;

typedef struct
{
    Type type;
    char *rep;
    char **op;
    char *variable;
    commandeStruct *cmd;
} cmdFor;

struct cmd_redirection
{
    cmd_simple *cmd;
    char *fichier;
    char *separateur;
};

struct commandeStruct
{
    Type type;
    cmd_simple *cmdSimple;
    cmd_pipe *pipe;
    cmdIf *cmdIf;
    cmdFor *cmdFor;
    commandeStruct **cmdsStruc;
    int nbCommandes;
};

#endif