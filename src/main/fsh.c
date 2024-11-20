#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <string.h>
#include "../../utils/cd.h"
#include "../../utils/pwd.h"
#include "../../utils/exit.h"
#include "../../utils/redirection.h"
#include "../../utils/prompt.h"
#include <stdbool.h>
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/for.h"
#include "../../utils/ftype.h"

#define ARG_MAX 512

int dernier_exit = 0; // pour initialiser la derniére valeur de retour

int main(void)
{
    char *input = malloc(MAX_INPUT);
    if (input == NULL)
    {
        perror("malloc");
        return 1;
    }

    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("malloc");
        return 1;
    }

    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        return 1;
    }
    rl_outstream = stderr;
    using_history();
    read_history("history.txt");

    int ret = 0;

    char *arg = malloc(ARG_MAX);
    char *cmd = malloc(ARG_MAX);
    char *arg_initial = arg;
    char *cmd_initial = cmd;

    if (arg == NULL || cmd == NULL)
    {
        perror("malloc");
        return 1;
    }

    while (1)
    {
        int r = prompt(chemin, input, &ret);
        gestion_cmd(input, arg, cmd);
        ges2(input);

        if (r == 1) // Ctrl-D pressed
        {
            dernier_exit = commande_exit(arg);
            printf("der = %d\n", dernier_exit);
            if (input != NULL)
            {
                free(input);
            }
            if (chemin != NULL)
            {
                free(chemin);
            }
            if (arg_initial != NULL)
            {
                free(arg_initial);
            }
            if (cmd_initial != NULL)
            {
                free(cmd_initial);
            }

            exit(dernier_exit);
        }
        ret = fsh(cmd, arg, input, chemin, dernier_exit);
        dernier_exit = ret;
    }
}
