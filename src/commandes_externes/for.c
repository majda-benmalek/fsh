#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include "../../utils/redirection.h"
#include "../../utils/gestion.h"
#include "../../utils/extern.h"
#include "../../utils/ftype.h"
#include <linux/limits.h>
#include "../../utils/commande.h"
#include "../../utils/freeStruct.h"
#define ARG_MAX 512



void change_var(char * name,char * variable ,commandeStruct *cmd,char * repName){
    // printf("dans  change var\n");
    // fflush(NULL);
    // if (cmd ->type == NULL){
    //     printf("le type  bien null hein ds change var\n");
    //     fflush(NULL);
    // }
    // else{
    //     printf(" ah bah my bad le type est pas nul = %d\n",cmd->type);
    //     fflush(NULL);
    // }
    // printf("variable = %s\n",variable);
    char * inter=malloc(strlen(variable)+2);// ? CA C PR AVOIR LE BON NOM DE VARIABLE +2 pr $ et le char 0
    strcpy(inter,"$");
    strcat(inter,variable);
    // printf("inter = %s\n",inter);
    // printf("cmd->cmdSimple->args[%d] = %s\n",2,cmd->cmdSimple->args[2]);
    switch (cmd->type)
    {
    case CMD_EXTERNE:
    // printf("chui ds cmd externe \n");
        int k = 0;
        while (cmd->cmdSimple->args[k]!=NULL){
            // printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
            if (strcmp(cmd->cmdSimple->args[k],inter) == 0){
                // Pr avoir le chemin complet faut mettre le nom du répertoire
                char *nom_complet = malloc(strlen(name)+strlen(repName)+2);// +2 pr / et '\0'
                strcpy(nom_complet,repName);
                strcat(nom_complet,"/");
                strcat(nom_complet,name);
                // printf("nom complet = %s\n",nom_complet);
                char * var_temp = realloc(cmd->cmdSimple->args[k],strlen(nom_complet));
                if (var_temp == NULL){
                    perror("realloc");
                    return;
                }
                cmd->cmdSimple->args[k] = var_temp;
                strcpy(cmd->cmdSimple->args[k],nom_complet);
                // strcpy(cmd->cmdSimple->args[i],name);
                // printf("ds change var après strcmy = %s\n",cmd->cmdSimple->args[k]);
            }
            k++;
        }
        break;
    case CMD_INTERNE:
        // printf("change var cmd interne \n");
        // fflush(NULL);
        int i = 0;
        while (cmd->cmdSimple->args[i]!=NULL){
            // if (strcmp(cmd->cmdSimple->args[i],variable) == 0){
            if (strcmp(cmd->cmdSimple->args[i],inter) == 0){
                // realloc(cmd->cmdSimple->args[i],sizeof(name));
                // Pr avoir le chemin complet faut mettre le nom du répertoire
                char *nom_complet = malloc(strlen(name)+strlen(repName)+2);// +2 pr / et '\0'
                strcpy(nom_complet,repName);
                strcat(nom_complet,"/");
                strcat(nom_complet,name);
                // printf("nom complet = %s",nom_complet);
                char * var_temp =realloc(cmd->cmdSimple->args[i],strlen(nom_complet)+1);
                if (var_temp == NULL){
                    perror("pb realloc");
                    return;
                }
                cmd->cmdSimple->args[i] = var_temp;
                strcpy(cmd->cmdSimple->args[i],nom_complet);
                // strcpy(cmd->cmdSimple->args[i],name);
                // printf("ds change var après strcmy = %s\n",cmd->cmdSimple->args[i]);
            }
            i++;
        }
        break;
    
    default:
        // printf("change var default \n");
        break;
    }
}


void restaurer_var(char * ancienne,char * nouveau,commandeStruct *cmd){
    // printf("dans restaure var\n");
    // printf("ancienne = %s\n",ancienne);
    // printf("nouveau = %s\n",nouveau);
    switch (cmd->type)
    {
    case CMD_EXTERNE:
        // printf("chui une vmd externe\n");
        int k = 0;
         while (cmd->cmdSimple->args[k]!=NULL){
            // printf("cmd->cmdSimple->args[%d] = %s\n",k,cmd->cmdSimple->args[k]);
            if (strcmp(cmd->cmdSimple->args[k],ancienne) == 0){
                cmd->cmdSimple->args[k]= NULL;
                cmd->cmdSimple->args[k] = malloc(strlen(nouveau)+2);
                strcpy(cmd->cmdSimple->args[k],"$");
                strcat(cmd->cmdSimple->args[k],nouveau);
                // Pr avoir le chemin complet faut mettre le nom du répertoire
                // char *nom_complet = malloc(strlen(name)+strlen(repName)+2);// +2 pr / et '\0'
                // strcpy(nom_complet,repName);
                // strcat(nom_complet,"/");
                // strcat(nom_complet,name);
                // printf("nom complet = %s\n",nom_complet);
                // strcpy(cmd->cmdSimple->args[k],nom_complet);
                // strcpy(cmd->cmdSimple->args[i],name);
                // printf("ds change var après strcmy = %s\n",cmd->cmdSimple->args[k]);
            }
            k++;
        }
        break;
    default:
        break;
    }
}
// TODO changer la fct en int car void pas ouf si pb de malloc realloc tt ça faut renvoyer un int qui dis que y'a un pb


// for i in rep { ls -l $i ; echo $i ; }

//TODO Si ca ce passe mal ft faire un truc
int boucle_for(cmdFor *cmdFor)
{
    // if (cmdFor ==NULL){
    //     printf("dans la boucle for cmdfor est null\n");
    // }
    // else{
    //     printf("cmd for var = %s\n",cmdFor->variable);
    //     printf("rep = %s\n",cmdFor->rep);
    // }
    // if (cmdFor->cmd[0]== NULL){
    //     printf("cmd for-> cmd est null\n");
    // }
    // else{
    //     printf(" c pas null\n");
    // }
    int dernier_exit=0; //TODO A CHANGER
    int ret=0; //TODO A CHANGER;
    DIR *dir = opendir(cmdFor->rep);
    if (dir == NULL)
    {
        perror("Erreur d'ouverture du repertoire");
        ret=1;
        return ret;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // printf("dans le while\n");
        if (entry->d_name[0] != '.')
        {
            // printf("entry->d_name ... = %s \n",entry->d_name);
            //? ici
            int nbr_cmd = 0;
            while (cmdFor->cmd[nbr_cmd] != NULL)
            {
                // printf("dans while entry->d_name ... = %s \n",entry->d_name);
                // printf(" dans while cmd for var = %s\n",cmdFor->variable);
                // if (cmdFor->cmd[nbr_cmd] == NULL){
                //     // printf("cmd for-> cmd est null\n");
                //     return ret;
                // }
                // else{
                //     printf(" c pas null le cmdfor -> cmd [nbr_cmd] \n");
                //     printf("type = %d\n",cmdFor->cmd[nbr_cmd]->type);
                // }
                // printf("avant le change var \n");
                // printf("entry->d_name ... = %s \n",entry->d_name);
                // printf("cmd for var = %s\n",cmdFor->variable);
                // if (cmdFor->cmd[nbr_cmd] == NULL){
                //     printf("c lui le pb\n");
                // }
                // else{
                //     printf("bh jsp c pas lui\n");
                // }
                //         nom du rep    var du for i/F    cmd à executer       path du rep
                // printf("entry->d_name = %s\n",entry->d_name);
                change_var(entry->d_name,cmdFor->variable, cmdFor->cmd[nbr_cmd],cmdFor->rep);
                ret = fsh("",&dernier_exit,cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL){
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }
                char * ancienne = malloc(strlen(entry->d_name)+strlen(cmdFor->rep)+2);
                strcpy(ancienne,cmdFor->rep);
                strcat(ancienne,"/");
                strcat(ancienne,entry->d_name);
                restaurer_var(ancienne,cmdFor->variable,cmdFor->cmd[nbr_cmd]);
                if (cmdFor->cmd[nbr_cmd] == NULL){
                    perror("pb ds le changement de var");
                    free_for(cmdFor);
                    return 1;
                }

                // change_var(cmdFor->variable,entry->d_name, cmdFor->cmd[nbr_cmd],cmdFor->rep);
                // if (ret < 0)
                // {
                //     perror("Erreur de fsh");
                //     closedir(dir);
                //     return ret;
                // }
                nbr_cmd=nbr_cmd+1;
            }
        }
    }

    closedir(dir);

    return ret;
}


//?
// char *ptr = (*cmdFor->cmd)->cmdSimple->args;
            // while ((ptr = strchr(ptr, '$')) != NULL)
            // {
            //     if (*(ptr + 1) == cmdFor->variable)
            //     {
            //         char chemin[1000];
            //         snprintf(chemin, sizeof(chemin), "%s/%s",cmdFor->rep, entry->d_name);
            //         memmove(ptr + strlen(chemin), ptr + 2, strlen(ptr + 2) + 1);
            //         memcpy(ptr, chemin, strlen(chemin));
            //         ptr += strlen(chemin);
            //     }
            //     else
            //     {
            //         ptr++;
            //     }
            // }





// int boucle_for(cmdFor *cmdFor)
// {
//     int dernier_exit=0; //TODO A CHANGER
//     int ret;
//     DIR *dir = opendir(cmdFor->rep);
//     if (dir == NULL)
//     {
//         perror("Erreur d'ouverture du repertoire");
//         ret=1;
//         return ret;
//     }
//     struct dirent *entry;
//     while ((entry = readdir(dir)) != NULL)
//     {
//         if (entry->d_name[0] != '.')
//         {
//             char *ptr = (*cmdFor->cmd)->cmdSimple->args;
//             while ((ptr = strchr(ptr, '$')) != NULL)
//             {
//                 if (*(ptr + 1) == cmdFor->variable)
//                 {
//                     char chemin[1000];
//                     snprintf(chemin, sizeof(chemin), "%s/%s",cmdFor->rep, entry->d_name);
//                     memmove(ptr + strlen(chemin), ptr + 2, strlen(ptr + 2) + 1);
//                     memcpy(ptr, chemin, strlen(chemin));
//                     ptr += strlen(chemin);
//                 }
//                 else
//                 {
//                     ptr++;
//                 }
//             }
//             int nbr_cmd = cmdFor->nbCommandes;
//             while (nbr_cmd>0)
//             {
//                 ret = fsh((*cmdFor->cmd)->cmdSimple->args,cmdFor->rep,&dernier_exit,cmdFor->cmd);
//                 if (ret < 0)
//                 {
//                     perror("Erreur de fsh");
//                     closedir(dir);
//                     return ret;
//                 }
//                 nbr_cmd--;
//             }
//         }
//     }

//     closedir(dir);

//     return ret;
// }


// int make_for(char *input, cmdFor *cmdFor){
//     //* --------- option----------
//     cmdFor->op = NULL;

//     // ? -------- Type ---------
//     cmdFor->type = FOR;

//     // * ------------------ variable ---------------
//     char *debut_variable = input + 4;
//     char *fin_variable = strstr(debut_variable, " in");
//     int ret = 0;

//     if (fin_variable != NULL)
//     {
//         int len = fin_variable - debut_variable;
//         if (len != 1)
//         {
//             perror("Erreur de syntaxe, la variable doit contenir un seul caractère");
//             return 1;
//         }
//         cmdFor->variable = *debut_variable;
//     }
//     else
//     {
//         perror("in attendu");
//         ret=1;
//         return ret;
//     }

//     // ? --------------- répertoire --------------
//     char *debut_rep_opt = strstr(input, "in ");
//     char *fin_rep_opt = strstr(input, " {");
//     char *fin_cmd = strstr(input, " }");

//     if (fin_rep_opt == NULL || fin_cmd == NULL || debut_rep_opt == NULL)
//     {
//         perror("Erreur de syntaxe");
//         ret=1;
//         return ret;
//     }

//     debut_rep_opt += 3;
//     int len_rep = fin_rep_opt - debut_rep_opt;
//     char rep[len_rep + 1];
//     strncpy(rep, debut_rep_opt, len_rep);
//     rep[len_rep] = '\0';
    
//     // * ------------ extraction des commandes ------------
//     char *debut_cmd = fin_rep_opt + 2;
//     int len_cmd = fin_cmd - debut_cmd;
//     char commandes[len_cmd + 1];
//     strncpy(commandes, debut_cmd, len_cmd);
//     commandes[len_cmd] = '\0';

//     // TODO APPPELER LA FCT FSH SUR COMMANDES
//     // gestion_cmd(commandes,cmdFor->cmd);
//     gestion_cmd(commandes, *(cmdFor->cmd));

//     return ret;
// }


//-----------

    // char * opt= strstr(rep,"-");

    // if (opt == NULL){
    //     cmdFor.rep= rep; // TODO for i in rep A {} le nom du répertoire sera "rep A" c pas bon
    // }else{

    // }

    // int rep_option(char *input, char *rep, char *opt){
    //     char *inter = strstr(input,"-");
    //     if (inter == NULL){
    //         rep=inter;
    //     }else{
    //         // int len
    //     }
    // }




//! Ancienne version


// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/wait.h>
// #include <dirent.h>
// #include <errno.h>
// #include <limits.h>
// #include "../../utils/redirection.h"
// #include "../../utils/gestion.h"
// #include "../../utils/extern.h"
// #include "../../utils/ftype.h"
// #include <linux/limits.h>
// #define ARG_MAX 512

// int boucle_for(char *input)
// {
//     /**
//      * for i in rep { ls -l $i ; echo $i ; }
//      * 1- ectraire la variable i
//      * 2- extraire rep
//      * 3- extraire les commande ls -l $i ; echo $i ;
//      * 4- pour chaque commande remplacer $i par le nom du fichier
//      * 5- utiliser strok pour separer les commandes avec le separateur " ;"
//      * 6- pour chaque commande verifier si c'est une redirection donc appeler la fonction redirection
//      * sinon utiliser execvp pour executer la commande
//      */

//     // pour la variable i
//     char var;
//     char *debut_variable = input + 4;
//     char *fin_variable = strstr(debut_variable, " in");
//     int ret = 0;

//     if (fin_variable != NULL)
//     {
//         int len = fin_variable - debut_variable;
//         if (len != 1)
//         {
//             perror("Erreur de syntaxe, la variable doit contenir un seul caractère");
//             return 1;
//         }
//         var = *debut_variable;
//     }
//     else
//     {
//         perror("in attendu");
//         ret=1;
//         return ret;
//     }
//     // extraction de rep
//     // for i in rep { ls -l $i ; echo $i ; }
//     // rep avant le " {" et aprés le " in"

//     char *debut_rep = strstr(input, "in ");
//     char *fin_rep = strstr(input, " {");
//     char *fin_cmd = strstr(input, " }");

//     if (fin_rep == NULL || fin_cmd == NULL || debut_rep == NULL)
//     {
//         perror("Erreur de syntaxe");
//         ret=1;
//         return ret;
//     }

//     debut_rep += 3;
//     int len_rep = fin_rep - debut_rep;
//     char rep[len_rep + 1];
//     strncpy(rep, debut_rep, len_rep);
//     rep[len_rep] = '\0';

//     // extraction des commandes
//     char *debut_cmd = fin_rep + 2;
//     int len_cmd = fin_cmd - debut_cmd;
//     char commandes[len_cmd + 1];
//     strncpy(commandes, debut_cmd, len_cmd);
//     commandes[len_cmd] = '\0';
//     // printf("cmd = %s\n var = %c\n rep = %s\n", commandes, var, rep);

//     // remplacer les $i

//     DIR *dir = opendir(rep);
//     if (dir == NULL)
//     {
//         perror("Erreur d'ouverture du repertoire");
//         ret=1;
//         return ret;
//     }
//     struct dirent *entry;
//     while ((entry = readdir(dir)) != NULL)
//     {

//         // pour ignorer les fichiers cachés
//         if (entry->d_name[0] != '.')
//         {
//             // printf("Fichier trouvé : %s\n", entry->d_name);
//             char commandes_modifiee[len_cmd + 1000];
//             strcpy(commandes_modifiee, commandes);
//             /* maintenant que j'ai le nom du fichier je vais chercher dans les commande si je trouve des occurence de $i pour ça je vais utiliser un autre pointeur ui va parcourir la liste*/
//             char *ptr = commandes_modifiee;
//             while ((ptr = strchr(ptr, '$')) != NULL)
//             {
//                 // verifier si c'est bien i
//                 if (*(ptr + 1) == var)
//                 {
//                     // deplacer la portion aprés la variable de pos + strlen d->d_name
//                     /*deplacer la chaine qui se situe apres la varible donc aprs pos+2 ($i) de longueur strlen(pos+2) donc ca nous donne le nombre de caractére a deplacer enfin les deplacer vers la position pos+strlen(d->d_name)*/
//                     char chemin[1000];

//                     snprintf(chemin, sizeof(chemin), "%s/%s", rep, entry->d_name);
//                     memmove(ptr + strlen(chemin), ptr + 2, strlen(ptr + 2) + 1);
//                     // puisque c'est deplacer on va copier
//                     memcpy(ptr, chemin, strlen(chemin));
//                     ptr += strlen(chemin);
//                 }
//                 else
//                 {
//                     ptr++;
//                 }
//             }
//             //  séparer les cmd avec le separateur ;
//             //  appliquer chaque commande
//             char *cmd = strtok(commandes_modifiee, ";");
//             char *cmd1 = cmd;
//             snprintf(cmd, strlen(cmd1), "%s", cmd1 + 1);

//             while (cmd != NULL)
//             {
//                 char *arg1 = malloc(ARG_MAX);
//                 char *cmd1 = malloc(ARG_MAX);
//                 gestion_cmd(cmd, arg1, cmd1);
//                 int dernier_exit = 0;
//                 char chemin_fsh[4096] = "";
//                 if (getcwd(chemin_fsh, 4096) == NULL)
//                 {
//                     perror("Erreur de getcwd");
//                     closedir(dir);
//                     free(arg1);
//                     free(cmd1);
//                     ret=1;
//                     return ret;
//                 }

//                 ret = fsh(cmd1, arg1, cmd, chemin_fsh, &dernier_exit);
//                 if (ret < 0)
//                 {
//                     perror("Erreur de fsh");
//                     closedir(dir);
//                     free(arg1);
//                     free(cmd1);
//                     return ret;
//                 }
//                 cmd = strtok(NULL, ";");
//             }
//         }
//     }

//     closedir(dir);

//     return ret;
// }
