# Documentation de comparaison

Date: 13 avril 2026

## Objectif

Ce document explique concretement la comparaison entre:
- le projet en racine (`/root/holbertonschool-simple_shell`)
- le projet reconstruit dans le dossier separe (`/root/holbertonschool-simple_shell/refait_shell`)
- le repo GitHub de reference: https://github.com/ValentinCA28/holbertonschool-simple_shell

## Reponse courte

1. Le projet racine et le projet `refait_shell` sont-ils strictement les memes ?
- Non.

2. Sont-ils globalement coherents fonctionnellement ?
- Oui, en grande partie.

3. Le projet `refait_shell` est-il un clone exact du repo GitHub de reference ?
- Non.

## Differences verifiees (racine vs refait_shell)

Sortie de comparaison (commande utilisee):

```bash
diff -qr --exclude=.git . refait_shell
```

Points constates:
- `AUTHORS` differ
- `README.md` differ
- `man_1_simple_shell` differ
- `shell.h` differ
- `shell.c` differ
- `main.c` differ
- `parser.c` differ
- `path.c` differ
- `execute.c` differ
- `builtins.c` differ
- binaire `hsh` present dans `refait_shell`

## Differences de comportement importantes

### 1) Prompt interactif
- Racine: `#cisfun$ `
- `refait_shell`: `($) `

Impact:
- Le rendu n'est pas identique entre les deux projets.

### 2) Builtin `exit` avec code
- Racine: `exit` simple gere; gestion de code de sortie plus limitee.
- `refait_shell`: `exit 42` retourne bien 42.

Impact:
- `refait_shell` est plus proche d'un comportement shell attendu sur ce point.

### 3) Propagation du status de commande
- Racine: retour de `waitpid` brut (moins exploitable tel quel).
- `refait_shell`: utilise `WIFEXITED`, `WEXITSTATUS`, `WIFSIGNALED`.

Impact:
- Le code de retour final est plus coherent dans `refait_shell`.

## Cohherence avec le repo GitHub de reference

Conclusion:
- Coherent sur le coeur du projet (simple shell, PATH, fork/execve, wait, builtins `exit`/`env`, mode interactif/non-interactif).
- Pas identique a 100% a la structure/fichiers visibles sur le repo GitHub (exemples: presence de fichiers comme `helpers.c` dans la version GitHub, absents ici).

## Validation technique effectuee sur `refait_shell`

Compilation:

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

Tests executes:

```bash
echo "/bin/ls" | ./hsh
echo "qwerty" | ./hsh
printf "/bin/echo ok\n/bin/echo done\n" | ./hsh
printf "exit 42\n" | ./hsh; echo EXIT_CODE:$?
```

Resultats observes:
- Execution commandes valides: OK
- Message not found: OK (`./hsh: 1: qwerty: not found`)
- Boucle multi-commandes: OK
- `exit 42`: OK (retour 42)

## Decision pratique

Si ton but est:

1. "Avoir la meme base fonctionnelle que le projet" 
- `refait_shell` est coherent.

2. "Avoir strictement le meme projet que la racine"
- Ce n'est pas le cas actuellement.

3. "Avoir strictement le meme etat que le repo GitHub de reference"
- Ce n'est pas le cas actuellement.

## Action recommandee selon ton besoin

- Besoin "checker Holberton" (fonctionnel):
  harmoniser uniquement les points de sortie/messages et valider Betty + leaks.

- Besoin "identique repo GitHub":
  aligner structure des fichiers, signatures de fonctions et docs fichier par fichier.
