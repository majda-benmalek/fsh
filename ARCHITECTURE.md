# ARCHITECTURE.md


## Introduction 
Ce projet implémente un shell personnalisé fsh, qui supporte des commandes internes simple comme : cd , exit , pwd... Ainsi que des commandes externes, des commandes structurées, des boucles for, des redirections ainsi que des piplines.

## Organisation du projet : 

### Arborescence :
```
├── AUTHORS.md                # Liste des auteurs
├── Makefile                  # Fichier pour compiler le projet
├── README.md                 # Documentation principale
├── src/                      # Code source principal
│   ├── commandes_externes/   # Gestion des commandes externes
│   ├── commandes_internes/   # Gestion des commandes internes
│   └── main/                 # Code principal               
├── utils/ # Fichiers d'en-tête pour le projet

```

### Structure de Données : 
Les commandes de notre projet sont organisées en plusieurs types de structures pour permettre une représentation flexible et extensible (dans le fichier `utils/commande.h`).Chaque commande peut être de type simple,Conditionnelle...

## 1. Type des commandes : 
Nous avons plusieurs types pour représenter une commande et chacun est représenté par des structures propres à chaque type: 
```c
typedef enum
{
    CMD_EXTERNE, 
    CMD_INTERNE,
    REDIRECTION,
    CMD_STRUCT,
    FOR,
    IF,
    PIPE
} Type;

```

## 2. `cmd_simple`: 
Elle représente les commandes simples. Une commande simple peut être soit une commande externe (par exemple `ls` ou `cat`), soit une commande interne du shell (par exemple `cd`, `exit`)
```c
typedef struct
{
    Type type; // le type de la commande (CMD_INTERNE, REDIRECTION, CMD_EXTERNE )
    char **args; // tableau des arguments de la commande
} cmd_simple;

```

## 3. `cmd_pipe`: 
Pour représenter les commandes de type `CMD_1 | CMD_2 | ... | CMD_N`.
```c
typedef struct
{
    Type type; 
    cmd_simple **commandes; 
    int nbCommandes;
} cmd_pipe;
```

### 3.2 Description des champs : 
* `type` : Le type de la commande, ici ça sera `PIPE` pour indiquer que c'est une commande avec des pipes.
* `**commandes` : Un tableau de pointeurs vers des structures `cmd_simple` , chaque élément représentant une commande simple dans le pipline.
* `nbCommandes` : Le nombre de commandes dans le pipline. 



## 4. `cmdIf`: 
Pour représenter les commandes conditionnelles de type `if TEST { CMD_1 } else { CMD_2 }`.

```c
typedef struct
{
    Type type;
    commandeStruct *test;
    commandeStruct *commandeIf;
    commandeStruct *commandeElse;
} cmdIf;
```

## 5. `cmdFor`: 
Permet de représenter les commandes de type `for F in REP [-A] [-r] [-e EXT] [-t TYPE] [-p MAX] { CMD }`.

## 6. `cmd_redirection`: 
Pour représenter les commandes de type redirection.


## 6. `commandeStruct`: 
Cette structure est l'élément central du projet, utilisée pour représenter différentes commandes. Elle contient des pointeurs vers d'autres structures représentant les autres types de commandes de notre projet : 
#### Définition
```c
struct commandeStruct {
    Type type;                          // Le type de la commande (par exemple, CMD_EXTERNE, CMD_INTERNE, etc.)
    cmd_simple *cmdSimple;              // Pointeur vers une commande simple (cmd_simple)
    cmd_pipe *pipe;                     // Pointeur vers une commande pipe (cmd_pipe)
    cmdIf *cmdIf;                       // Pointeur vers une commande de type "if" (cmdIf)
    cmdFor *cmdFor;                     // Pointeur vers une commande de type "for" (cmdFor)
    cmd_redirection *cmdRed;            // Pointeur vers une commande de redirection (cmd_redirection)
    commandeStruct **cmdsStruc;         // Tableau de pointeurs vers plusieurs commandes imbriquées (pour gérer les structures complexes)
    int nbCommandes;                    // Nombre de commandes imbriquées dans cmdsStruc
};
```





### Contenu des repertoires : 
#### `src/commandes_externes` :
Ce dossier contient les fichiers responsables de l'exécution des commandes externent qui nécessitent un appel a la fonction `exec`.

- **`cmdStructuree.c`** : Responsable de l'éxecution des commandes de type : `CMD1 ; CMD2 ; ..... ; CMDn`.
- **`extern.c`** : Contient la fonction qui fait le seul et unique appel à la fonction `execvp` .
- **`for.c`** : Implémente la  commande structurée `for F in REP [-A] [-r] [-e EXT] [-t TYPE] [-p MAX] { CMD }`
- **`if.c`** : Responsable de l'implémentation de la commande de type `if TEST { CMD_1 } else { CMD_2 }`
- **`pipe.c`** : Responsable de l'implémentation des commandes de type `CMD_1 | CMD_2 | ... | CMD_N`

#### `src/commandes_externes` :
Ce dossier gère les commandes iternes du shell : cd , exit , pwd , ftype.

#### `src/main` :
Ce dossier représente le coeur du projet, il contient le fichier exécutable ainsi les utilitaires de gestion. 

- **`commandeStructuree.c`** : Responsable du traitement de l'input pour les commandes de type `CMD1 ; CMD2 ; ..... ; CMDn` pour extraire les commandes.
- **`freeStruct.c`** : Fichier responsable de la libération de mémoires pour les structures utilisées dans le projet.
- **`fsh.c`** : Fichier exécutable, contient la boucle principale.
- **`gestion.c`** : Contient deux fonctions trés importantes : 
##### 1. `gestion_cmd`:  
Cette fonction analyse l'entrée et détécte son type pour remplir `commandeStruct` en fonction de son type (if, for, redirection, pipe, ou commande simple) afin de pouvoir l'exécuter.
##### 2. `fsh`:  
Cette fonction teste le type de la structure donnée (CMD_EXTERNE, CMD_INTERNE,REDIRECTION,CMD_STRUCT,FOR,IF,PIPE) et appelle la fonction appropriées pour la traiter.
- **`gestionStruct.c`** : Contient les fonctions responsables du remplissage des structures.
- **`prompt.c`** : Responsable de l'affichage du prompt.
- **`siganux.c`** : Contient les fonctions nécessaires pour la gestion des signaux.


#### `utils/` :
Regroupe tous les fichiers d'en-tête.












