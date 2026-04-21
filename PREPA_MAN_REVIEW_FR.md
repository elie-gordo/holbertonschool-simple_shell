# Preparation man review - simple_shell

Ce guide est concu pour reviser le code fichier par fichier et anticiper les questions.

## 1) Parcours de code (ordre conseille)

1. shell.h - contrat global, struct shell_s, prototypes
2. main.c - point d'entree, signal handler, init/cleanup
3. shell_loop.c - boucle REPL + mode fichier
4. input.c - _getline custom (buffer + read)
5. tokenizer.c - decoupage en tokens (sans strtok)
6. operators.c - gestion ;  &&  ||, execution de segments
7. variables.c - expansion $?, $$, $VAR + suppression commentaires
8. path.c - resolution commande via PATH
9. execute.c - fork/execve/waitpid
10. builtins.c - dispatch + exit + env
11. builtins2.c - setenv, unsetenv, cd
12. builtins3.c - alias
13. builtins4.c - help, history
14. environ.c - gestion dynamique de l'environnement
15. history.c - chargement/sauvegarde historique
16. helpers.c - _atoi_custom, _itoa, _is_number

## 2) shell.h - ce qu'il faut dire

- role: contrat global entre tous les modules
- struct shell_s: centralise tout l'etat du shell (name, count, status,
  interactive, should_exit, env, aliases, history)
- includes systeme necessaires au projet
- prototypes de chaque fonction exposee
- defines: BUF_SIZE, MAX_ALIASES, HIST_MAX
- extern g_signal: seule variable globale, pour le signal handler
- include guard pour eviter inclusions multiples

Question piege:
Pourquoi un struct plutot que des variables globales ?
Reponse:
Pour encapsuler l'etat, eviter les globals (Betty les interdit sauf g_signal),
et pouvoir passer tout le contexte en un seul pointeur.

## 3) main.c - ce qu'il faut dire

- sigint_handler: ecrit '\n', positionne g_signal, c'est tout
  (pas de printf car pas async-signal-safe)
- init_shell: memset + copie de l'env + chargement historique
- cleanup_shell: sauvegarde historique + liberation de toutes les ressources
- main: init, signal, dispatch (file_mode ou shell_loop), cleanup

Question piege:
Pourquoi write() et pas printf() dans le signal handler ?
Reponse:
printf n'est pas async-signal-safe, write est garanti safe dans un handler.

## 4) shell_loop.c - explication ligne par ligne

- _strip_newline: retire le '\n' final de la ligne lue
- shell_loop:
  - affiche prompt uniquement si interactif
  - reset g_signal avant chaque lecture
  - _getline lit la ligne
  - si -1 et g_signal: Ctrl+C, on continue (nouveau prompt)
  - si -1 sans signal: EOF (Ctrl+D), on sort
  - count++ a chaque ligne
  - strip newline, skip lignes vides
  - add_history avant traitement
  - strip_comments, skip si vide apres
  - handle_operators execute la ligne
  - if should_exit: break
  - '\n' final en interactif pour sortie propre

- file_mode:
  - ouvre le fichier, erreur 127 si introuvable
  - meme boucle sans prompt
  - ferme le fd en fin

Question piege:
Pourquoi add_history AVANT strip_comments ?
Reponse:
Pour conserver la ligne originale dans l'historique, commentaires inclus.

## 5) input.c - custom _getline

- _fill_buf: quand le buffer statique est vide, lit BUF_SIZE octets avec read()
- _ensure_cap: double la taille du lineptr si necessaire
- _getline:
  - variables statiques: buf[BUF_SIZE], pos, len, prev_fd
  - si le fd change (ex: historique puis stdin), reset du buffer
  - alloue lineptr si NULL
  - boucle: remplir buffer, copier char par char jusqu'a '\n' ou EOF
  - retourne nombre de chars ou -1

Question piege:
Pourquoi des variables statiques ?
Reponse:
Pour conserver le buffer entre les appels. read() peut lire plus qu'une ligne,
les chars restants doivent etre disponibles au prochain appel.

## 6) tokenizer.c - sans strtok

- _count_words: compte les mots, gere les quotes simples (in_q flag)
- _get_word: extrait un mot, respecte les guillemets simples
- tokenize: alloue le tableau, remplit avec _get_word
- free_tokens: libere chaque token puis le tableau

Question piege:
Pourquoi gerer les guillemets simples ?
Reponse:
Pour le built-in alias: "alias ll='ls -la'" doit rester un seul token.

## 7) operators.c - ;  &&  ||

- _find_op: parcourt la ligne, retourne le type d'operateur et sa position
- _build_alias_line: reconstruit la ligne apres expansion d'alias
- exec_segment: pipeline complet pour un segment:
  1. expand_variables (remplace $?, $$, $VAR)
  2. tokenize
  3. expansion alias si args[0] est un alias
  4. run_builtin ou execute_cmd
- handle_operators: decoupe par operateurs, execute chaque segment
  selon la logique ; (toujours), && (si succes), || (si echec)

Question piege:
Comment fonctionne la logique && / || ?
Reponse:
On maintient un flag "run". Apres chaque operateur:
- ; : run = 1 (toujours)
- && : run = (status == 0)
- || : run = (status != 0)
Le segment suivant n'est execute que si run est vrai.

## 8) variables.c - expansion

- strip_comments: met '\0' au premier # precede d'un espace
- _append: ajoute une chaine au buffer resultat (realloc si necessaire)
- _expand_dollar: gere $? $$ et $VARIABLE
- expand_variables: construit une nouvelle chaine avec toutes les expansions

Question piege:
Que se passe-t-il si $VARIABLE n'existe pas ?
Reponse:
_getenv retourne NULL, _append ne fait rien, la variable est remplacee par "".

## 9) path.c - resolution PATH

- _build_path: construit "repertoire/commande" avec sprintf
- _next_dir: extrait le prochain repertoire de PATH (sans strtok, manuellement)
- _search_path: boucle sur les repertoires, teste access(X_OK)
- find_command: dispatch entre chemin absolu et recherche PATH

Question piege:
Pourquoi ne pas fork si commande introuvable ?
Reponse:
Exigence task 0.3: eviter un fork inutile. On verifie AVANT avec find_command.

## 10) execute.c - fork/execve/waitpid

- print_not_found: format "prog: line: cmd: not found"
- _child_exec: remet SIGINT a SIG_DFL, appelle execve, _exit(127) si echec
- execute_cmd: find_command, fork, _child_exec dans enfant, waitpid dans parent

Question piege:
Pourquoi signal(SIGINT, SIG_DFL) dans l'enfant ?
Reponse:
Le parent ignore SIGINT (Ctrl+C ne quitte pas le shell), mais l'enfant
doit pouvoir etre interrompu normalement.

## 11) builtins.c - dispatch principal

- builtin_exit: gere exit sans arg, exit N, exit abc (erreur Illegal number)
- builtin_env: affiche sh->env (notre copie dynamique)
- _is_exit: simple strcmp helper
- run_builtin: if/else chain sur args[0], retourne 1 si traite, 0 sinon

## 12) builtins2.c - setenv, unsetenv, cd

- builtin_setenv: verifie 2 args, delegue a _setenv
- builtin_unsetenv: verifie 1 arg, delegue a _unsetenv
- _do_cd: getcwd ancien, chdir, mise a jour OLDPWD et PWD
- builtin_cd: gere cd (HOME), cd - (OLDPWD + print), cd DIR

Question piege:
Pourquoi mettre a jour PWD et OLDPWD ?
Reponse:
Pour que cd - fonctionne et que $PWD soit coherent avec le repertoire reel.

## 13) builtins3.c - alias

- _find_alias_idx: recherche par prefixe "name=" dans le tableau
- get_alias_value: retourne la valeur sans les quotes
- _set_alias: strdup de la definition complete
- builtin_alias: gere les 3 cas (print all, print one, set)

## 14) builtins4.c - help et history

- _print_help_all: liste tous les built-ins avec description courte
- _print_help_one: details pour un built-in specifique
- builtin_help: dispatch all vs specific
- builtin_history: affiche les entrees avec numeros de ligne

## 15) environ.c - gestion dynamique de l'environnement

- copy_environ: malloc + strdup de chaque entree de extern environ
- _getenv: parcours lineaire avec strncmp + verification du '='
- _setenv: recherche, remplace ou ajoute (realloc le tableau)
- _unsetenv: recherche, free, shift du dernier element
- free_environ: free de chaque entree puis du tableau

Question piege:
Pourquoi ne pas utiliser le environ global directement ?
Reponse:
Pour pouvoir modifier l'env (setenv/unsetenv) proprement avec des allocations
controlees et passer notre copie a execve.

## 16) history.c - persistance

- _get_hist_path: construit "$HOME/.simple_shell_history"
- load_history: ouvre le fichier, lit avec _getline, stocke dans tableau
- save_history: ecrit chaque entree dans le fichier (open + write)
- add_history: ajoute au tableau, shift si > HIST_MAX (4096)
- free_history: libere toutes les entrees

## 17) helpers.c - utilitaires

- _is_number: verifie que la chaine est un entier valide (digits, optionnel +/-)
- _atoi_custom: conversion string -> int avec gestion du signe
- _itoa: conversion int -> string alloue (via sprintf)

## 18) Questions pieges frequentes

Q: Difference entre /bin/ls et ls ?
R: /bin/ls est un chemin explicite; ls doit etre resolu via PATH.

Q: Pourquoi args doit finir par NULL ?
R: C'est le format attendu par execve pour savoir ou s'arrete argv.

Q: Comment gerez-vous Ctrl+D ?
R: _getline retourne -1 (read retourne 0 a EOF), g_signal est 0, on sort.

Q: Comment gerez-vous Ctrl+C ?
R: Signal handler ecrit '\n', g_signal=1. Si on etait dans _getline,
   read retourne -1 (EINTR), _getline retourne -1, le loop voit g_signal
   et continue au lieu de quitter.

Q: Que retourne votre shell en cas de commande introuvable ?
R: 127, conforme a la convention shell.

Q: Pourquoi ne pas utiliser system() ?
R: Le sujet impose fork/execve pour comprendre la gestion processus.

Q: Pourquoi une seule variable globale ?
R: Betty interdit les globals. L'exception est g_signal car un signal handler
   ne peut pas recevoir de parametres arbitraires.

Q: Comment fonctionne l'expansion d'alias ?
R: Dans exec_segment, apres tokenisation, on verifie si args[0] est un alias.
   Si oui, on reconstruit la ligne complete (valeur alias + args restants)
   et on re-tokenise.

## 19) Script de demonstration review

Compilation:
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

Tests basiques:
echo "/bin/echo OK" | ./hsh
echo "ls -l" | ./hsh
echo "qwerty" | ./hsh
echo "env" | ./hsh | head -3
echo "exit 98" | ./hsh ; echo $?

Tests avances:
echo "ls /var ; echo done" | ./hsh
echo "ls /hbtn && echo oui" | ./hsh
echo "ls /hbtn || echo non" | ./hsh
printf 'echo $?\necho $$\n' | ./hsh
printf 'echo hello # comment\n' | ./hsh
echo "cd /tmp" | ./hsh
./hsh /tmp/test_commands.sh

## 20) Checklist finale avant passage

- le projet compile sans warning
- README, man et AUTHORS presents et a jour
- style Betty respecte (1 warning attendu: g_signal)
- pas de fuite memoire (toutes les ressources liberees dans cleanup_shell)
- messages d'erreur au bon format (identiques a /bin/sh)
- les 8 built-ins operationnels (exit, env, setenv, unsetenv, cd, alias, help, history)
- operateurs ;  &&  || fonctionnels
- variables $? $$ $VAR expandees
- commentaires # geres
- mode fichier fonctionnel (./hsh filename)
- Ctrl+C ne quitte pas le shell
- Ctrl+D quitte proprement
- historique charge/sauvegarde dans ~/.simple_shell_history
- explication claire de chaque fichier et de chaque syscall
