# Documentation des notions - simple_shell

Ce document regroupe les notions theoriques a maitriser pour expliquer le projet.

## 1) Comment fonctionne un shell

Un shell suit une boucle (REPL):
1. afficher un prompt (si interactif)
2. lire une ligne (via _getline avec buffer + read)
3. supprimer les commentaires (#)
4. decouper par operateurs (;  &&  ||)
5. pour chaque segment: expander les variables, tokenizer, resoudre le PATH
6. executer (built-in ou fork/execve)
7. attendre la fin du processus enfant
8. recommencer

## 2) Fonction vs system call

- Fonction: code de bibliotheque en espace utilisateur (ex: strlen, printf).
- System call: passage en noyau pour demander un service OS (ex: fork, execve, waitpid, read, write).

Syscalls utilises dans ce projet:
- `read` : lecture brute depuis un file descriptor (base de _getline)
- `write` : affichage du prompt, sortie signal handler
- `fork` : duplication du processus
- `execve` : remplacement de l'image du processus par un binaire
- `waitpid` : attente de la fin du processus enfant
- `access` : verification X_OK avant fork
- `chdir` : changement de repertoire (built-in cd)
- `getcwd` : recuperation du repertoire courant
- `open` / `close` : lecture fichier (mode fichier, historique)
- `signal` : gestion de SIGINT (Ctrl+C)
- `getpid` : recuperation du PID (expansion $$)
- `stat` / `fstat` : informations sur un fichier

## 3) PID et PPID

- PID: identifiant unique du processus courant.
- PPID: PID du parent qui a cree ce processus.

Dans ce projet:
- le shell parent cree un enfant avec fork;
- l'enfant execute la commande via execve;
- le parent attend avec waitpid.

## 4) execve

execve remplace le code du processus courant par le binaire cible.

Signature:
int execve(const char *path, char *const argv[], char *const envp[]);

Points importants:
- ne retourne pas en cas de succes;
- retourne -1 en cas d'erreur;
- argv doit finir par NULL;
- envp est notre copie dynamique de l'environnement (gere par environ.c).

## 5) fork + waitpid

fork:
- retourne -1 en erreur;
- retourne 0 dans l'enfant;
- retourne PID enfant dans le parent.

waitpid:
- permet au parent de recuperer le statut enfant;
- WIFEXITED(status) pour sortie normale;
- WEXITSTATUS(status) pour code de sortie;
- WIFSIGNALED(status) pour signal (ex: Ctrl+C sur enfant).

## 6) PATH

PATH est une variable env du type:
/usr/local/bin:/usr/bin:/bin

Strategie (path.c):
- si la commande contient '/', on teste directement access(X_OK);
- sinon on parcourt chaque repertoire de PATH avec _next_dir (sans strtok);
- on construit dir/commande avec _build_path;
- on retourne le premier chemin executable;
- si rien trouve, on retourne NULL et fork n'est PAS appele.

## 7) Parsing et tokenisation

Le projet utilise un tokenizer custom (tokenizer.c) sans strtok:
- _count_words: compte les mots (gere les guillemets simples pour alias)
- _get_word: extrait chaque mot un par un avec gestion des quotes
- tokenize: produit le tableau argv termine par NULL

Exemple:
"ls -l /tmp" devient:
- args[0] = "ls"
- args[1] = "-l"
- args[2] = "/tmp"
- args[3] = NULL

## 8) Custom _getline (input.c)

Le projet interdit l'usage de la fonction getline de la libc.

Implementation:
- _fill_buf: lit BUF_SIZE octets d'un coup avec read()
- _ensure_cap: agrandit le buffer lineptr si necessaire avec realloc
- _getline: assemble les caracteres un par un depuis le buffer statique
  jusqu'a trouver '\n' ou EOF
- variables statiques pour conserver l'etat du buffer entre les appels
- gestion du changement de fd (prev_fd) pour historique puis stdin

## 9) Operateurs ;  &&  || (operators.c)

Traitement sequentiel de gauche a droite:
- `;` : execute toujours le segment suivant
- `&&` : execute le suivant seulement si le precedent a reussi (status == 0)
- `||` : execute le suivant seulement si le precedent a echoue (status != 0)

La fonction _find_op parcourt la ligne pour trouver le prochain operateur.
Chaque segment est execute via exec_segment qui gere:
1. expansion des variables ($?, $$, $VAR)
2. expansion des alias
3. tokenisation
4. dispatch built-in ou commande externe

## 10) Environnement dynamique (environ.c)

L'environnement est copie dans un tableau dynamique a l'initialisation:
- copy_environ: duplique le tableau extern environ
- _getenv: recherche par nom dans notre copie
- _setenv: ajoute ou remplace une variable (realloc si nouvelle)
- _unsetenv: supprime une variable (shift du tableau)
- free_environ: libere tout le tableau

Cela permet a setenv/unsetenv de modifier l'environnement et de le passer
a execve sans toucher au environ global.

## 11) Variables (variables.c)

Expansion a la volee avant tokenisation:
- `$?` : remplace par le code de sortie de la derniere commande
- `$$` : remplace par le PID du shell (getpid)
- `$VARIABLE` : remplace par la valeur de l'env (_getenv)
- `$` seul ou suivi d'un caractere non-alnum : conserve le $ literal

La fonction _expand_dollar construit le resultat caractere par caractere
dans un buffer dynamique.

## 12) Commentaires (variables.c)

strip_comments supprime tout ce qui suit un `#` si:
- il est en debut de ligne; ou
- il est precede d'un espace ou tabulation.

## 13) Signaux (main.c)

Le shell gere SIGINT (Ctrl+C):
- sigint_handler ecrit un '\n' sur stdout et positionne g_signal = 1
- g_signal est la seule variable globale du projet (volatile sig_atomic_t)
- dans shell_loop, si _getline retourne -1 et g_signal est actif,
  on continue la boucle (nouveau prompt) au lieu de quitter
- les processus enfants recoivent SIG_DFL pour etre correctement tues

## 14) Built-ins

Traites par run_builtin AVANT la resolution PATH:
- exit [status] : quitte avec should_exit, status optionnel, erreur si non-entier
- env : affiche sh->env ligne par ligne
- setenv VAR VALUE : delegation a _setenv
- unsetenv VAR : delegation a _unsetenv
- cd [dir] : chdir + mise a jour PWD/OLDPWD, gestion cd - et cd sans arg
- alias [name[='value']] : gestion du tableau aliases du shell
- help [builtin] : affichage d'aide
- history : affichage de l'historique avec numeros de ligne

## 15) Historique (history.c)

- load_history: lit ~/.simple_shell_history au demarrage via _getline
- add_history: ajoute chaque ligne lue a un tableau dynamique (max 4096)
- save_history: ecrit le tableau dans le fichier a la sortie
- free_history: libere toutes les entrees

## 16) Alias (builtins3.c)

Stockage: tableau de chaines au format "name='value'" dans sh->aliases.
- _find_alias_idx: recherche par nom
- _set_alias: creation ou remplacement
- get_alias_value: extraction de la valeur (suppression des quotes)
- builtin_alias: gestion de toutes les formes (print all, print one, set)

Expansion dans exec_segment: si args[0] est un alias, on reconstruit
la ligne avec la valeur de l'alias + les arguments restants.

## 17) Mode fichier (shell_loop.c)

Usage: ./hsh filename
- file_mode ouvre le fichier en lecture seule
- lit chaque ligne avec _getline(fd)
- pas de prompt affiche
- traitement identique (comments, operateurs, execution)
- retourne le dernier status

## 18) Structure shell_t

Le struct shell_s centralise tout l'etat du shell:
- name: nom du programme (argv[0]) pour les messages d'erreur
- count: compteur de lignes pour les messages d'erreur
- status: dernier code de sortie ($?)
- interactive: isatty(STDIN_FILENO)
- should_exit: flag de sortie pour exit
- env/env_count: copie dynamique de l'environnement
- aliases/alias_count: tableau d'alias
- hist/hist_count: tableau d'historique

## 19) Contraintes Holberton

- compilation: gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
- style Betty (max 40 lignes/fonction, max 5 fonctions/fichier)
- documentation de chaque fonction
- include guards dans les headers
- une seule variable globale autorisee (g_signal pour les signaux)
- pas de fuite memoire
