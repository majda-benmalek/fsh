#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

extern int dernier_exit;

int commande_exit(char *valeur)
{
    int code;
    if (strlen(valeur) == 0)
    {
        code = dernier_exit;
    }
    else
    {
        code = atoi(valeur);
    }
    char *msg = malloc(MAX_INPUT);
    sprintf(msg, "Exit avec code %d\n", code);
    write(2, msg, strlen(msg));
    if (msg != NULL)
    {
        free(msg);
    }
    return code;
}
