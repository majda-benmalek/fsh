#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>

#define ARG_MAX 512

int cmd_extern(char *input)
{
    char **arg = malloc(ARG_MAX * sizeof(char *));
    if (arg == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        // return 1;
        return -4;
    }

    char *copy = strdup(input);
    if (copy == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        free(arg);
        // return 1;
        return -4;
    }

    char *token = strtok(copy, " ");
    int count = 0;
    while (token != NULL)
    {
        arg[count] = strdup(token);
        if (arg[count] == NULL)
        {
            perror("erreur d'allocation de memoire pour count");
            goto error;
        }
        count++;
        token = strtok(NULL, " ");
    }
    arg[count] = NULL;

    int child_pid;
    int status = 0;
    switch (child_pid = fork())
    {
    case -1:
        perror("fork");
        goto error;
        break;
    case 0:
        if (execvp(arg[0], arg) == -1)
        {
            perror("redirect_exec");
            free(copy);
            for (int i = 0; i < count; i++)
            {
                free(arg[i]);
            }
            free(arg);
            exit(1);
        }
        break;
    default:
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) // 0 C TRUE 1 C FALSE
        {
            int exit_status = WEXITSTATUS(status);
            free(copy);
            for (int i = 0; i < count; i++)
            {
                free(arg[i]);
            }
            free(arg);
            return exit_status;
            // return 0;
        }
        else
        {
            goto error;
        }
    }
error:
    free(copy);
    for (int i = 0; i < count; i++)
    {
        free(arg[i]);
    }
    free(arg);
    return -4;
}
