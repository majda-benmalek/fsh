#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int dernier_exit;

void commande_exit(char *valeur)
{
    int code = valeur ? atoi(valeur) : dernier_exit;
    printf("Exit avec le code %d\n", code);
    exit(code);
}

void cmd_exit(char *arg)
{
    if (strlen(arg) >= 1)
    {
        commande_exit(arg);
    }
    else
    {
        commande_exit(NULL);
    }
}