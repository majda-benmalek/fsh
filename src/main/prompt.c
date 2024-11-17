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

int compter_chiffres(int nombre)
{
    int count = 0;
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

void decoupe(char *chemin)
{
    // size_t size_path = strlen(chemin);
    // size_t size = ;
    // if (size <= 30)
    // {
    //     return NULL;
    // }
    // else
    // {
    // char *new_prompt = malloc(size * sizeof(char));
    // if (new_prompt == NULL)
    // {
    //     perror("malloc");
    //     return NULL;
    // }
    // int ouverture = 5;
    // int fermeture = 4;
    // int where = 0;
    // snprintf(new_prompt, ouverture + 1 + fermeture + 1, "%s", prompt);
    // where = ouverture + 1 + fermeture;
    // int c = compter_chiffres(ret);
    // snprintf(new_prompt + where, c + 2, "%s", prompt + where); //+2 pour ] et '\0'
    // where += c + 1;

    // size_t val = (ouverture + 1 + fermeture) * 2 + c + 1;
    // char *new_path = chemin + (size_path - val);
    // snprintf(new_prompt + where, strlen(new_path) + 4, "...%s", new_path); // pour ... et '\0'
    // int where2 = where + size_path;
    // where += strlen(new_path) + 3;
    // snprintf(new_prompt + where, ouverture + 2, "%s", prompt + where2); //+2 pour $ et '\0'
    // where2 += ouverture + 1;
    // where += ouverture + 1;
    // snprintf(new_prompt + where, fermeture + 2, "%s", prompt + where2);
    // new_prompt[strlen(new_prompt) - 1] = ' ';

    // return new_prompt;
    if (strlen(chemin) > 25)
    {
        chemin[strlen(chemin)] = '\0';
        int len = strlen(chemin);
        int enlever = len - 25;
        strcpy(chemin, chemin + enlever);
        chemin[25] = '\0';
        for (size_t i = 0; i < 3; i++)
        {
            chemin[i] = '.';
        }
    }
}
//}

int prompt(char *chemin, char *input, int *ret)
{
    input[0] = '\0';
    char readline_prompt[512];
    char *vert = "\001\033[32m\002";
    char *reset_color = "\001\033[00m\002";
    char *rouge = "\001\033[91m\002";
    char *bleu = "\001\033[34m\002";
    decoupe(chemin);
    if (*ret == 0)
    {
        sprintf(readline_prompt, "%s[%s%d]%s%s$%s ", vert, reset_color, *(ret), chemin, bleu, reset_color);
    }
    else
    {
        sprintf(readline_prompt, "%s[%s%d]%s%s$%s ", rouge, reset_color, *(ret), chemin, bleu, reset_color);
    }

    char *line = readline(readline_prompt);
    if (line != NULL)
    {
        sprintf(input, "%s", line);
        add_history(input);
        write_history("history.txt");
    }
    else if (line == NULL)
    {
        return 1;
    }
    return 0;
}
