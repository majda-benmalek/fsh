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

char *decoupe(char *prompt)
{
    size_t size = strlen(prompt);
    if (size <= 23)
    {
        return NULL;
    }
    else
    {
        char *new_prompt = malloc( size * sizeof(char));
        if (new_prompt == NULL)
        {
            perror("malloc");
            return NULL;
        }
        size_t debut_prompt = size - 23;
        snprintf(new_prompt, 27 , "...%s", prompt + debut_prompt);
        return new_prompt;
    }
}

void enlever_home(char *chemin, const char *home)
{
    char *position = strstr(chemin, home);
    if (position == chemin) // Si 'home' est trouvé au début de 'chemin'
    {
        size_t home_len = strlen(home);
        size_t chemin_len = strlen(chemin);
        memmove(chemin, chemin + home_len, chemin_len - home_len + 1); // +1 pour inclure le '\0'
    }
}

void prompt(char *chemin, char *input, bool ok)
{

    input[0] = '\0';
    enlever_home(chemin, getenv("HOME"));
    char *prompt = decoupe(chemin);
    if (prompt != NULL)
    {
        sprintf(chemin, "%s", prompt);
        free(prompt);
    }

    char readline_prompt[512];
    if (ok)
    {
        snprintf(readline_prompt, sizeof(readline_prompt), "\033[32m[\033[0m0]%s\033[34m$ \033[0m", chemin);
    }
    else
    {
        snprintf(readline_prompt, sizeof(readline_prompt), "\033[91m[\033[0m1]%s\033[34m$ \033[0m", chemin);
    }

    char *line = readline(readline_prompt);
    if (line != NULL)
    {
        sprintf(input, "%s", line);
        add_history(input);
        free(line);
    }
    write_history("history");
}