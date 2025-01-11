#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "../../utils/commande.h"
#include "../../utils/gestionStruct.h"
#include "../../utils/gestion.h"
#include "../../utils/freeStruct.h"
#include "linux/limits.h"

int dterminer_flags(const char *separateur)
{
    if (strcmp(separateur, ">>") == 0 || strcmp(separateur, "2>>") == 0)
    {
        return O_WRONLY | O_CREAT | O_APPEND;
    }
    else if (strcmp(separateur, ">") == 0 || strcmp(separateur, "2>") == 0)
    {
        return O_WRONLY | O_CREAT | O_EXCL;
    }
    else if (strcmp(separateur, "<") == 0)
    {
        return O_RDONLY;
    }
    else if (strcmp(separateur, ">|") == 0 || strcmp(separateur, "2>|") == 0)
    {
        return O_WRONLY | O_CREAT | O_TRUNC;
    }
    else
    {
        perror("separateur non reconnu");
        return 1;
    }
}

int ouvrir_fichier(const char *fichier, const char *separateur)
{

    int flags = dterminer_flags(separateur);
    if (flags == -1)
    {
        return 1;
    }
    int fd = open(fichier, flags, 0644);
    return fd;
}

int sauvgarde_descripteurs(int descripteur, int *copie)
{
    if (!copie)
    {
        perror("pointeur null pour le copie");
        return 1;
    }
    *copie = dup(descripteur);
    if (*copie < 0)
    {
        perror("erreur lors de la sauvegarde");
        return 1;
    }
    return 0;
}

int restauration_descipteur(int copie, int descripteur)
{
    if (dup2(copie, descripteur) < 0)
    {
        perror("erreur lors de la restauration");
        close(copie);
        return 1;
    }
    close(copie);
    return 0;
}

int duplication_fd(int fd, char *separateur)
{
    int cible = 1;
    if (strcmp(separateur, ">>") == 0 || strcmp(separateur, ">") == 0 || strcmp(separateur, ">|") == 0)
    {
        cible = STDOUT_FILENO;
    }
    else if (strstr(separateur, "2") != NULL)
    {
        // perror("ya un 2");
        cible = STDERR_FILENO;
    }
    else
    {
        cible = STDIN_FILENO;
    }

    if (dup2(fd, cible) < 0)
    {
        perror("erreur duplication fd");
        return 1;
    }
    return 0;
}

int redirection(cmd_simple *cmd)
{
    int ret = 0;
    cmd_redirection *cmdredirect = cmd->red;
    if (cmdredirect == NULL || cmdredirect->cmd == NULL || cmdredirect->fichier == NULL || cmdredirect->separateur == NULL)
    {
        perror("Arguments pour la redirection manquants");
        return 1;
    }

    char *fichier = cmdredirect->fichier;
    char *separateur = cmdredirect->separateur;

    int fd = ouvrir_fichier(fichier, separateur);
    if (fd == -1)
    {
        perror("pipeline_run");
        return 1;
    }
    int copie_stdout = -1, copie_stdin = -1, copie_stderr = -1;
    if (sauvgarde_descripteurs(STDOUT_FILENO, &copie_stdout) != 0)
    {
        perror("sauvgarde descripteurs");
        close(fd);
        return 1;
    }
    if (sauvgarde_descripteurs(STDIN_FILENO, &copie_stdin) != 0)
    {
        perror("sauvgarde descripteurs");
        close(fd);
        return 1;
    }

    if (sauvgarde_descripteurs(STDERR_FILENO, &copie_stderr) != 0)
    {
        perror("sauvegarde descripteurs");
        close(fd);
        return 1;
    }

    if (duplication_fd(fd, separateur) != 0)
    {
        perror("duplication fd");
        restauration_descipteur(copie_stdout, STDOUT_FILENO);
        restauration_descipteur(copie_stdin, STDIN_FILENO);
        restauration_descipteur(copie_stderr, STDERR_FILENO);
        close(fd);
        return 1;
    }
    close(fd);

    int dernier_exit = 0;
    char *chemin = malloc(PATH_MAX);
    if (chemin == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire");
        close(copie_stdin);
        close(copie_stdout);
        close(copie_stderr);
        return 1;
    }
    if (getcwd(chemin, PATH_MAX) == NULL)
    {
        perror("getcwd");
        free(chemin);
        close(copie_stdin);
        close(copie_stdout);
        close(copie_stderr);
        return 1;
    }

    commandeStruct *cmdstr = remplissage_cmdStruct(CMD_STRUCT, NULL, NULL, NULL, NULL, NULL, 0, NULL);
    gestion_cmd(cmdredirect->cmd->args, cmdstr);

    ret = fsh(chemin, &dernier_exit, cmdstr);
    // ! pour évité les pertes de mémoire
    freeCmdStruct(cmdstr);
    if (chemin)
        free(chemin);
    if (restauration_descipteur(copie_stdout, STDOUT_FILENO) != 0 ||
        restauration_descipteur(copie_stdin, STDIN_FILENO) != 0 || restauration_descipteur(copie_stderr, STDERR_FILENO) != 0)
    {
        perror("restaurer descripteur");
        return 1;
    }
    return ret;
}