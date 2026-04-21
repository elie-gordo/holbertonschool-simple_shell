# Preparation man review - simple_shell

Ce guide est concu pour reviser le code fichier par fichier et anticiper les questions.

## 1) Parcours de code (ordre conseille)

1. shell.h
2. main.c
3. shell.c
4. parser.c
5. builtins.c
6. path.c
7. execute.c

## 2) shell.h - ce qu'il faut dire

- role: contrat global entre tous les modules;
- includes systeme necessaires au projet;
- prototypes de chaque fonction exposee;
- include guard pour eviter inclusions multiples.

Question piege:
Pourquoi declarer extern char **environ ?
Reponse attendue:
Pour reutiliser l'environnement global du processus si besoin dans les modules.

## 3) main.c - ce qu'il faut dire

- main recupere argv/envp puis appelle simple_shell;
- aucune logique metier ici: separation claire des responsabilites.

Question piege:
Pourquoi ne pas coder toute la boucle dans main ?
Reponse:
Pour garder main minimal et rendre le code maintenable/testable.

## 4) shell.c - explication ligne par ligne (niveau review)

- initialisation line=NULL, len=0 pour getline;
- isatty(STDIN_FILENO) pour distinguer interactif/non-interactif;
- prompt affiche uniquement en interactif;
- getline lit la ligne complete;
- si -1: EOF/erreur, sortie boucle;
- cmd_count incremente a chaque ligne lue;
- suppression du '\n' final;
- ignore les lignes vides;
- parse_line cree args;
- handle_builtin traite exit/env;
- sinon execute_command pour les commandes externes;
- free_args libere le conteneur;
- should_exit arrete proprement la boucle;
- free(line) en fin de fonction.

Question piege:
Pourquoi conserver line entre les iterations au lieu de free a chaque tour ?
Reponse:
getline peut reutiliser/reallouer le buffer, ce qui est efficace et standard.

## 5) parser.c - points clefs

- copie de line dans count_tokens car strtok modifie la chaine;
- allocation args de taille total + 1;
- args termine par NULL pour execve;
- free_args libere seulement le conteneur, pas les tokens.

Question piege:
Pourquoi ne pas strdup chaque token ?
Reponse:
Ici on reutilise le buffer line pour limiter les allocations; tokens pointent dedans.

## 6) builtins.c - points clefs

- comparaison de args[0] avec strcmp;
- exit: active should_exit;
- env: parcourt envp et imprime ligne par ligne;
- retourne 1 si built-in traite sinon 0.

Question piege:
Pourquoi traiter les built-ins avant PATH/fork ?
Reponse:
Ce sont des commandes internes au shell, elles n'ont pas de binaire externe.

## 7) path.c - points clefs

- commande avec '/': test direct access(X_OK);
- sinon lecture PATH depuis envp;
- duplication de PATH (strdup) car strtok est destructif;
- boucle sur les repertoires separes par ':';
- construction dir/cmd;
- retour premier chemin executable.

Question piege:
Pourquoi ne pas faire fork avant find_command ?
Reponse:
Exigence task 0.3: ne pas fork si commande introuvable.

## 8) execute.c - points clefs

- si find_command renvoie NULL: erreur not found + code 127;
- fork;
- enfant: execve(cmd_path, args, envp);
- parent: waitpid;
- interpretation du status (WIFEXITED/WEXITSTATUS).

Question piege:
Pourquoi _exit dans l'enfant apres echec execve ?
Reponse:
Pour quitter immediatement le processus enfant sans effets de bord stdio.

## 9) Questions pieges frequentes (et reponses)

Q: Difference entre /bin/ls et ls ?
R: /bin/ls est un chemin explicite; ls doit etre resolu via PATH.

Q: Pourquoi args doit finir par NULL ?
R: C'est le format attendu par execve pour savoir ou s'arrete argv.

Q: Comment gerez-vous Ctrl+D ?
R: getline retourne -1; on sort de la boucle proprement et on free line.

Q: Que retourne votre shell en cas de commande introuvable ?
R: 127, conforme a la convention shell.

Q: Pourquoi ne pas utiliser system() ?
R: Le sujet impose fork/execve pour comprendre la gestion processus.

## 10) Mini script de demonstration review

Compilation:
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

Tests:
echo "/bin/echo OK" | ./hsh
echo "ls -l" | ./hsh
echo "qwerty" | ./hsh
echo "env" | ./hsh
echo "exit" | ./hsh

## 11) Checklist finale avant passage

- le projet compile sans warning;
- README, man et AUTHORS presents;
- pas de fuite memoire evidente;
- messages d'erreur au bon format;
- built-ins exit/env operationnels;
- explication claire de chaque fichier et de chaque syscall.
