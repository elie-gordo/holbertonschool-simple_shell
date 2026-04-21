# Documentation des notions - simple_shell (tasks 0-6)

Ce document regroupe les notions theoriques a maitriser pour expliquer le projet.

## 1) Comment fonctionne un shell

Un shell suit une boucle (REPL) :
1. afficher un prompt (si interactif)
2. lire une ligne (via `getline`)
3. decouper la ligne en tokens (via `strtok`)
4. si la commande est un built-in (`exit`, `env`), l'executer dans le shell
5. sinon, resoudre le chemin via `PATH`
6. `fork` puis `execve` dans l'enfant, `waitpid` dans le parent
7. recommencer

## 2) Fonction vs system call

- Fonction : code de bibliotheque en espace utilisateur (ex : `strlen`, `printf`, `getline`).
- System call : passage en noyau pour demander un service OS (ex : `fork`, `execve`, `waitpid`, `read`, `write`).

Syscalls utilises dans ce projet :
- `write` : affichage du prompt
- `fork` : duplication du processus
- `execve` : remplacement de l'image du processus par un binaire
- `waitpid` : attente de la fin du processus enfant
- `access` : verification `X_OK` avant fork (task 0.3)

## 3) PID et PPID

- PID : identifiant unique du processus courant.
- PPID : PID du parent qui a cree ce processus.

Dans ce projet :
- le shell parent cree un enfant avec `fork` ;
- l'enfant execute la commande via `execve` ;
- le parent attend avec `waitpid`.

## 4) execve

`execve` remplace le code du processus courant par le binaire cible.

Signature :

    int execve(const char *path, char *const argv[], char *const envp[]);

Points importants :
- ne retourne pas en cas de succes ;
- retourne -1 en cas d'erreur ;
- `argv` doit finir par `NULL` ;
- `envp` est la variable globale `environ` du projet.

## 5) fork + waitpid

`fork` :
- retourne -1 en erreur ;
- retourne 0 dans l'enfant ;
- retourne PID enfant dans le parent.

`waitpid` :
- permet au parent de recuperer le statut enfant ;
- `WIFEXITED(status)` : sortie normale ;
- `WEXITSTATUS(status)` : code de sortie ;
- `WIFSIGNALED(status)` : tue par signal.

## 6) PATH

`PATH` est une variable d'environnement du type :

    /usr/local/bin:/usr/bin:/bin

Strategie (`path.c`) :
- si la commande contient un `/`, on teste directement `access(X_OK)` ;
- sinon, on parcourt chaque repertoire de `PATH` avec `strtok` ;
- on construit `dir/commande` avec `sprintf` ;
- on retourne le premier chemin executable trouve ;
- si rien n'est trouve, on retourne `NULL` et `fork` n'est PAS appele.

## 7) Parsing et tokenisation

Le projet utilise `strtok` de la libc avec les delimiteurs espace et tabulation.

`parse_line` (`parser.c`) :
- `count_tokens` : compte les mots en travaillant sur une copie (car `strtok` modifie sa source) ;
- allocation du tableau `args` (taille = nombre de tokens + 1 pour le `NULL` final) ;
- remplissage via `strtok` sur la ligne originale ;
- les pointeurs stockes dans `args` pointent directement dans le buffer de `getline`, pas de `strdup` par token.

Exemple :

    "ls -l /tmp"

devient :

    args[0] = "ls"
    args[1] = "-l"
    args[2] = "/tmp"
    args[3] = NULL

## 8) Lecture de la ligne

Le projet utilise `getline` de la libc, qui gere automatiquement l'allocation du buffer.
On le libere avec un seul `free(line)` a la fin de la boucle principale.

## 9) Built-ins

Traites par `handle_builtin` AVANT la resolution `PATH` :
- `exit` : positionne `should_exit = 1` pour casser la boucle.
- `env` : parcourt la variable globale `environ` et affiche chaque entree.

`handle_builtin` retourne `1` si le built-in a ete traite, `0` sinon.
Si `0`, l'appelant tente d'executer la commande via `execute_cmd`.

## 10) execute_cmd (execute.c)

1. `find_command(args[0])` pour resoudre le chemin.
2. Si `NULL` : appelle `print_not_found` et retourne `127` (PAS de fork).
3. Sinon : `fork`.
4. Enfant : `execve(cmd_path, args, environ)` ; si retour, `perror` + `_exit(127)`.
5. Parent : `waitpid`, libere `cmd_path`, retourne le status.

## 11) Codes de sortie

- `0` : succes.
- `127` : commande introuvable.
- `128 + n` : tue par le signal `n`.
- Sinon : le code que la commande a renvoye.

## 12) Format des messages d'erreur

Conforme a `/bin/sh` :

    ./hsh: 1: qwerty: not found

Le `1` est le numero de ligne (compteur `cmd_count` incremente a chaque commande lue).

## 13) Structure du projet

- `shell.h` : prototypes, includes, `extern char **environ`.
- `main.c` : point d'entree, delegue a `shell_loop`.
- `shell.c` : boucle REPL principale (`shell_loop`).
- `parser.c` : `parse_line`, `free_args`, `count_tokens`.
- `path.c` : `find_command`, `get_path_value`, `build_full_path`, `search_in_path`.
- `execute.c` : `execute_cmd`, `print_not_found`.
- `builtins.c` : `handle_builtin`, `print_env`.

## 14) Contraintes Holberton

- compilation : `gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh`
- style Betty (max 40 lignes par fonction, max 5 fonctions par fichier)
- documentation de chaque fonction (en-tete style Betty)
- include guards dans le header
- pas de variable globale (l'`environ` de la libc ne compte pas)
- pas de fuite memoire
