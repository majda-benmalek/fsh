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

char *decoupe(char *prompt, int ret, char *chemin)
{
    size_t size_path = strlen(chemin);
    size_t size = size_path + 5;
    if (size <= 30)
    {
        return NULL;
    }
    else
    {
        char *new_prompt = malloc(size * sizeof(char));
        if (new_prompt == NULL)
        {
            perror("malloc");
            return NULL;
        }
        int ouverture = 5;
        int fermeture = 4;
        int where = 0;
        snprintf(new_prompt, ouverture + 2, "%s", prompt); //+2 pour [ et '\0'
        where = ouverture + 1;
        snprintf(new_prompt + where, fermeture + 1, "%s", prompt + where);
        where += fermeture;
        int c = compter_chiffres(ret);
        snprintf(new_prompt + where, c + 2, "%s", prompt + where); // +2 pour ] et '\0'
        where += c + 1;
        char *new_path = chemin + (size_path - 21);
        snprintf(new_prompt + where, strlen(new_path) + 4, "...%s", new_path); // pour ... et '\0'
        int where2 = where + size_path;
        where += strlen(new_path) + 3;
        snprintf(new_prompt + where, ouverture + 2, "%s", prompt + where2); //+2 pour $ et '\0'
        where2 += ouverture + 1;
        where += ouverture + 1;
        snprintf(new_prompt + where, fermeture + 2, "%s", prompt + where2);

        return new_prompt;
    }
}

void prompt(char *chemin, char *input, int *ret)
{
    input[0] = '\0';
    char readline_prompt[512];
    if (*ret == 0)
    {
        sprintf(readline_prompt, "\033[32m[\033[0m%d]%s\033[34m$\033[0m ", *(ret), chemin);
    }
    else
    {
        sprintf(readline_prompt, "\033[91m[\033[0m%d]%s\033[34m$\033[0m ", *(ret), chemin);
    }

    char *prompt = decoupe(readline_prompt, *ret, chemin);
    if (prompt != NULL)
    {
        sprintf(readline_prompt, "%s", prompt);
        free(prompt);
    }

    char *line = readline(readline_prompt);
    if (line != NULL  && strlen(line) > 0)
    {
        sprintf(input, "%s", line);
        add_history(input);
    }
}
