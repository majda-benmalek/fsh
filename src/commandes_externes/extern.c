#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>

int cmd_extern(char *input)
{
    char **arg = malloc(ARG_MAX * sizeof(char *));
    if (arg == NULL)
    {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        return -1;
    }
    printf("input : %s\n", input);

    char *copy = strdup(input);
    if (copy == NULL)
    {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        free(arg);
        return -1;
    }

    printf("copy : %s\n", copy);
    char *token = strtok(copy, " ");

    int count = 0;
    while (token != NULL)
    {
        arg[count] = strdup(token);
        if (arg[count] == NULL)
        {
            fprintf(stderr, "Erreur d'allocation de mémoire\n");
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
    //affiché arg
    for (int i = 0; i < count; i++)
    {
        printf("arg[%d] : %s\n", i, arg[i]);
    }
    int child_pid;
    int status;
    switch (child_pid = fork())
    {
    case -1:
        perror("fork");
        free(copy);
        for (int i = 0; i < count; i++)
        {
            free(arg[i]);
        }
        free(arg);
        return 1;
    case 0:
        if (execvp(arg[0], arg) == -1)
        {
            perror("execvp");
            free(copy);
            for (int i = 0; i < count; i++)
            {
                free(arg[i]);
            }
            free(arg);
            exit(EXIT_FAILURE);
        }
    default:
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exit_status = WEXITSTATUS(status);
            return exit_status;
        }
    }

    free(copy);
    for (int i = 0; i < count; i++)
    {
        free(arg[i]);
    }
    free(arg);
    return 0;
}

// int main()
// {
//     char *input = "ls -l ";

//     char* chemin = malloc(PATH_MAX * sizeof(char));
//     if (chemin == NULL)
//     {
//         fprintf(stderr, "Erreur d'allocation de mémoire\n");
//         return EXIT_FAILURE;
//     }
//     getcwd(chemin, PATH_MAX);
//     printf("chemin : %s\n", chemin);
//     if (cmd_extern(input) == -1)
//     {
//         fprintf(stderr, "Erreur lors de l'exécution de la commande externe\n");
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }
// printf("copy : %s\n", copy);
// printf("token : %s\n", token);
// printf("cmd : %s\n", cmd);
// printf("input : %s\n", input);