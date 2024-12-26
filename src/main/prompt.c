#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <stdbool.h>
#include "../../utils/signaux.h"

int compter_chiffres(int nombre)
{
    int count;
    if (nombre > 0)
    {
        count = 0;
    }else{
        count = 1;
    }

    if (nombre == 0)
    {
        return 1;
    }
    if (nombre < 0)
    {
        (nombre) = -(nombre);
    }
    while (nombre != 0)
    {
        (nombre) /= 10;
        count++;
    }

    return count;
}

char *decoupe(char *chemin, int *ret)
{
    char *new_path = malloc(PATH_MAX);
    strcpy(new_path, chemin);
    if (strlen(new_path) > (30 - compter_chiffres(*ret) + 4))
    {
        new_path[strlen(new_path)] = '\0';
        int len = strlen(new_path);
        int out = compter_chiffres(*ret) + 4;
        int enlever = len - (30 - out);
        strcpy(new_path, new_path + enlever);
        new_path[30 - out] = '\0';
        for (size_t i = 0; i < 3; i++)
        {
            new_path[i] = '.';
        }
    }
    return new_path;
}

int prompt(char *chemin, char *input, int *ret)
{
    input[0] = '\0';
    char readline_prompt[512];
    char *vert = "\001\033[32m\002";
    char *reset_color = "\001\033[00m\002";
    char *rouge = "\001\033[91m\002";
    char *bleu = "\001\033[34m\002";
    char *new = decoupe(chemin, ret);
    if (*ret == 0)
    {
        sprintf(readline_prompt, "%s[%s%d]%s%s$%s ", vert, reset_color, *(ret), new, bleu, reset_color);
    }else if(*ret == -255){
       sprintf(readline_prompt, "%s[%sSIG]%s%s$%s ", rouge, reset_color,  new, bleu, reset_color);
    }
    else
    {
        sprintf(readline_prompt, "%s[%s%d]%s%s$%s ", rouge, reset_color, *(ret), new, bleu, reset_color);
    }
    free(new);
    rl_initialize();
    char *line = readline(readline_prompt);

    if (line != NULL)
    {
        sprintf(input, "%s", line);
        input[strlen(input)] = '\0';
        add_history(input);
        free(line);
    }
    else
    {
        return 1;
    }
    return 0;
}
