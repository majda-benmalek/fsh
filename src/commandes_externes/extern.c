#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>

#define ARG_MAX 512

int child(char** arg);

int cmd_extern(char *input)
{
    char **arg = malloc(ARG_MAX * sizeof(char *));
    if (arg == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        return 1;
    }

    char *copy = strdup(input);
    if (copy == NULL)
    {
        write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
        free(arg);
        return 1;
    }

    char *token = strtok(copy, " ");

    int count = 0;
    while (token != NULL)
    {
        arg[count] = strdup(token);
        if (arg[count] == NULL)
        {
            write(2, "Erreur d'allocation de mémoire\n", strlen("Erreur d'allocation de mémoire\n"));
            free(copy);
            for (int i = 0; i < count; i++)
            {
                free(arg[i]);
            }
            free(arg);
            return 1;
        }
        count++;
        token = strtok(NULL, " ");
    }
    arg[count] = NULL;
    // affiché arg
    //  for (int i = 0; i < count; i++)
    //  {
    //      printf("arg[%d] : %s\n", i, arg[i]);
    //  }
    // int child_pid;
    // int status;
    // switch (child_pid = fork())
    // {
    // case -1:
    //     perror("fork");
    //     free(copy);
    //     for (int i = 0; i < count; i++)
    //     {
    //         free(arg[i]);
    //     }
    //     free(arg);
    //     return 1;
    // case 0:
    //     if (execvp(arg[0], arg) == -1)
    //     {
    //         perror("execvp");
    //         free(copy);
    //         for (int i = 0; i < count; i++)
    //         {
    //             free(arg[i]);
    //         }
    //         free(arg);
    //         exit(EXIT_FAILURE);
    //     }
    // default:
    //     waitpid(child_pid, &status, 0);
    //     if (WIFEXITED(status))
    //     {
    //         int exit_status = WEXITSTATUS(status);
    //         return exit_status;
    //     }
    // }

    if(child(arg)>0){
        perror("child");
        free(copy);
        for (int i = 0; i < count; i++)
        {
            free(arg[i]);
        }
        free(arg);
        return 1;
    }

    free(copy);
    for (int i = 0; i < count; i++)
    {
        free(arg[i]);
    }
    free(arg);
    return 0;
}

int child( char** arg)
{
    int child_pid;
    int status;
    switch (child_pid = fork())
    {
    case -1:
        perror("fork");
        return 1;
    case 0:
        if (execvp(arg[0], arg) == -1)
        {
            perror("execvp");
            return 1;
        }
    default:
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
    }
    return 0;
}