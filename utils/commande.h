#ifndef COMMANDE_H
#define COMMANDE_H


typedef enum {
    CMD_EXTERNE,
    CMD_INTERNE,
    CMD_STRUCT,
    FOR,
    IF,
    REDIRECTION,
    PIPE
} Type;

typedef struct{
    Type type;
    char** args;
} cmd_simple;

typedef struct{
    Type type;
    cmd_simple ** commandes ;
    int nbCommandes;
} pipe;

typedef struct {
    Type type ; 
    pipe* test ; 
    commandeStruct ** commandeIf ;
    commandeStruct ** commandeElse ;
} cmdIf ;

typedef struct{
    Type type;
    char* rep;
    char** op;
    char variable ; 
    int nbCommandes;
    commandeStruct** cmd;
} cmdFor;

typedef struct{
    Type type;
    cmd_simple* cmdSimple;
    pipe* pipe;
    cmdIf * cmdIf;
    cmdFor * cmdFor;
    int nbCommandes;
} commandeStruct;





#endif