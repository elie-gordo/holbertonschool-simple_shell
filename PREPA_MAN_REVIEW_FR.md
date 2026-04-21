# Preparation man review - simple_shell (tasks 0-6)

Ce guide permet de reviser le code fichier par fichier et d'anticiper les questions.

## 1) Parcours de code (ordre conseille)

1. `shell.h` — contrat global, prototypes
2. `main.c` — point d'entree
3. `shell.c` — boucle REPL
4. `parser.c` — tokenisation (avec `strtok`)
5. `path.c` — resolution de la commande via `PATH`
6. `execute.c` — `fork` / `execve` / `waitpid`
7. `builtins.c` — dispatcher + `exit` + `env`

## 2) shell.h — ce qu'il faut dire

- Role : contrat global entre tous les modules.
- Declare `extern char **environ` (fourni par la libc).
- Prototypes de chaque fonction exposee :
  - `shell.c` : `shell_loop`
  - `parser.c` : `parse_line`, `free_args`
  - `path.c` : `find_command`
  - `execute.c` : `execute_cmd`, `print_not_found`
  - `builtins.c` : `handle_builtin`
- Includes systeme : `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<unistd.h>`, `<sys/types.h>`, `<sys/wait.h>`.
- Include guard `#ifndef SHELL_H ... #endif`.

Question piege :
Pourquoi `extern char **environ` et pas `getenv` ?
Reponse :
Pour pouvoir parcourir l'environnement dans le built-in `env` et chercher la variable `PATH` dans `path.c`.

## 3) main.c — ce qu'il faut dire

- `main(int ac, char **av)` :
  - ignore `ac` avec `(void)ac;`
  - appelle et retourne `shell_loop(av[0])`
- On passe `av[0]` pour que les messages d'erreur commencent par le nom du programme (`./hsh`).

Question piege :
Pourquoi passer `av[0]` ?
Reponse :
Pour le format d'erreur conforme a `/bin/sh` : `./hsh: 1: cmd: not found`.

## 4) shell.c — explication pas a pas

Boucle `shell_loop(char *prog_name)` :
1. `interactive = isatty(STDIN_FILENO)` : teste si on est dans un terminal.
2. Boucle infinie :
   - si interactif, `write(STDOUT_FILENO, "$ ", 2)` (prompt) ;
   - `nread = getline(&line, &len, stdin)` ;
   - si `nread == -1` (EOF / Ctrl+D) : `break` ;
   - `cmd_count++` ;
   - strip du `\n` final ;
   - skip si ligne vide ;
   - `parse_line(line)` pour obtenir `args` ;
   - `handle_builtin(args, &should_exit)` ;
   - sinon `execute_cmd(args, prog_name, cmd_count)` qui met a jour `status` ;
   - `free_args(args)` ;
   - si `should_exit`, `break`.
3. En sortie : `\n` final si interactif, `free(line)`, retourne `status`.

Question piege :
Pourquoi `write` et pas `printf` pour le prompt ?
Reponse :
`write` est un syscall direct sans bufferisation : le prompt s'affiche immediatement.

## 5) parser.c — tokenisation

- `count_tokens(char *line)` (static) : fait un `strdup` car `strtok` modifie sa source, boucle `strtok(copy, " \t")` pour compter, puis `free(copy)`.
- `parse_line(char *line)` :
  - compte les tokens, retourne `NULL` si 0 ;
  - `malloc` le tableau de taille `total + 1` ;
  - remplit avec `strtok(line, " \t")` sur la ligne originale ;
  - termine par `args[i] = NULL` (requis par `execve`).
- `free_args(char **args)` : ne free que le tableau, pas les chaines (elles pointent dans le buffer de `getline`).

Question piege :
Pourquoi `strdup` dans `count_tokens` ?
Reponse :
`strtok` place des `\0` dans la chaine source ; il faut travailler sur une copie pour ne pas detruire la ligne avant le second `strtok` dans `parse_line`.

## 6) path.c — resolution `PATH`

- `build_full_path(dir, cmd)` : alloue `len = strlen(dir) + strlen(cmd) + 2`, fait `sprintf(full, "%s/%s", dir, cmd)`.
- `get_path_value()` : parcourt `environ`, cherche l'entree qui commence par `"PATH="`, retourne un pointeur sur le caractere qui suit le `=`.
- `search_in_path(cmd, path_copy)` : `strtok(path_copy, ":")`, pour chaque `dir` : construit le chemin complet, teste `access(full, X_OK)`, retourne la premiere correspondance, sinon `free` et continue.
- `find_command(cmd)` :
  - si `cmd` contient un `/` : teste directement `access(X_OK)`, retourne `strdup(cmd)` si OK, sinon `NULL` ;
  - sinon : `get_path_value`, `strdup` de `PATH`, `search_in_path`, `free` du copy.

Question piege :
Pourquoi ne pas fork si la commande est introuvable ?
Reponse :
Exigence de la tache 0.3 : eviter un fork inutile. On appelle `find_command` AVANT `fork` ; si `NULL`, on affiche l'erreur et on retourne `127`.

## 7) execute.c — fork / execve / wait

- `print_not_found(prog, line, cmd)` : `fprintf(stderr, "%s: %d: %s: not found\n", prog, line, cmd)`.
- `execute_cmd(args, prog_name, cmd_count)` :
  1. `cmd_path = find_command(args[0])` ;
  2. si `NULL` : `print_not_found`, retourne `127` (pas de fork) ;
  3. `pid = fork()` ;
     - `-1` : `perror`, `free(cmd_path)`, retourne `1` ;
     - `0` (enfant) : `execve(cmd_path, args, environ)` ; si retour, `perror` + `_exit(127)` ;
     - parent : `waitpid(pid, &wstatus, 0)`, `free(cmd_path)` ;
  4. retourne `WEXITSTATUS(wstatus)`, ou `128 + WTERMSIG` si tue par signal, ou `1` en fallback.

Question piege :
Pourquoi `_exit` et pas `exit` dans l'enfant ?
Reponse :
`exit` flush les buffers stdio dupliques par `fork` (double flush possible). `_exit` termine immediatement sans toucher aux buffers.

## 8) builtins.c — dispatch

- `print_env()` (static) : boucle `while (environ[i])` et `printf("%s\n", environ[i])`.
- `handle_builtin(args, should_exit)` :
  - `strcmp(args[0], "exit") == 0` : `*should_exit = 1` ; retourne `1`.
  - `strcmp(args[0], "env") == 0` : `print_env()` ; retourne `1`.
  - sinon : retourne `0`.

Question piege :
Pourquoi un built-in plutot qu'un binaire externe pour `exit` ?
Reponse :
`exit` doit terminer le processus shell lui-meme, pas un enfant. Si c'etait un externe, on ferait `fork + execve` et seul l'enfant quitterait, pas le shell parent.

## 9) Questions pieges frequentes

Q : Difference entre `/bin/ls` et `ls` ?
R : `/bin/ls` est un chemin explicite, on teste directement `access`. `ls` doit etre resolu via `PATH`.

Q : Pourquoi `args` doit finir par `NULL` ?
R : C'est le contrat d'`execve` pour savoir ou s'arrete `argv`.

Q : Comment gerez-vous Ctrl+D ?
R : `getline` retourne `-1` quand `read` lit 0 octet (EOF) ; le loop sort avec `break`.

Q : Que retourne votre shell si la commande est introuvable ?
R : `127`, conforme a la convention shell.

Q : Pourquoi ne pas utiliser `system()` ?
R : Le sujet impose `fork` / `execve` pour comprendre la gestion des processus.

Q : Avez-vous des variables globales ?
R : Aucune dans notre code. On utilise `environ` de la libc, qui est declaree `extern` mais pas definie par nous.

## 10) Script de demonstration review

Compilation :

    gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

Tests basiques :

    echo "/bin/echo OK" | ./hsh
    echo "ls -l" | ./hsh
    echo "qwerty" | ./hsh
    echo "env" | ./hsh | head -3
    echo "exit" | ./hsh ; echo $?

Comparaison automatique avec `/bin/sh` :

    ./review_proof.sh

## 11) Checklist finale avant passage

- le projet compile sans warning (`-Wall -Werror -Wextra -pedantic -std=gnu89`)
- `README.md`, `man_1_simple_shell` et `AUTHORS` presents et a jour
- style Betty respecte (zero warning / zero error)
- pas de fuite memoire (Valgrind clean)
- messages d'erreur au bon format (identiques a `/bin/sh`)
- built-ins `exit` et `env` operationnels
- resolution `PATH` operationnelle
- pas de fork si commande introuvable
- code de sortie `127` pour commande introuvable
- Ctrl+D quitte proprement
- explication claire de chaque fichier et de chaque syscall
