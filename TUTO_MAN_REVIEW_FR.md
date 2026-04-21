# Tuto man review — simple_shell

Guide pas a pas pour demontrer en live que ton shell fonctionne correctement.
A suivre dans l'ordre, chaque section est un "point a prouver" au reviewer.

Pre-requis : tu es dans le dossier du projet, ton terminal est pret.

---

## Etape 0 — Preparer le terrain

Efface le binaire existant pour prouver qu'on recompile a froid :

```
rm -f hsh
ls
```

Tu dois voir tes sources `.c`, le `.h`, `README.md`, `AUTHORS`, `man_1_simple_shell`.

---

## Etape 1 — Compilation stricte (exigence Holberton)

Commande officielle du projet :

```
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

**A dire :**
> "Le projet compile sans warning ni erreur avec les flags imposes par Holberton :
> `-Wall` (tous les warnings), `-Werror` (warning = erreur), `-Wextra` (warnings
> supplementaires), `-pedantic` (respect strict de la norme), `-std=gnu89` (norme C89 GNU)."

Verification :

```
ls -la hsh
```

Tu dois voir le binaire `hsh` executable.

---

## Etape 2 — Conformite Betty

```
betty *.c *.h
```

Tu dois voir uniquement la liste des fonctions, **aucun message** `ERROR` ou `WARNING`.

**A dire :**
> "Le code respecte la norme Betty : max 40 lignes par fonction, max 5 fonctions
> par fichier, documentation de chaque fonction, include guard, indentation par tabulations."

---

## Etape 3 — Prompt interactif (tache 0.1)

Lance le shell :

```
./hsh
```

Tu vois le prompt `$ `. Tape :

```
/bin/ls
```

**A dire :**
> "Le shell affiche un prompt, lit une ligne avec `getline`, la parse, fait
> `fork` + `execve`, attend l'enfant avec `waitpid`, puis reaffiche le prompt."

---

## Etape 4 — Ctrl+D quitte proprement (tache 0.1)

Dans le shell interactif, appuie sur **Ctrl+D**.

**A dire :**
> "Ctrl+D envoie EOF a stdin. `getline` retourne `-1`, la boucle `break`,
> le shell libere la memoire et retourne le dernier status."

---

## Etape 5 — Commande avec arguments (tache 0.2)

```
./hsh
$ /bin/ls -l /tmp
$ /bin/echo hello world
$ exit
```

**A dire :**
> "Le parser utilise `strtok` avec ` ` et `\t` comme delimiteurs. Il renvoie
> un tableau `args` termine par `NULL`, qui est le contrat attendu par `execve`."

---

## Etape 6 — Resolution PATH (tache 0.3)

```
echo "ls" | ./hsh
```

**A dire :**
> "La commande `ls` ne contient pas de `/`, donc on parcourt les repertoires
> de la variable `PATH` et on retourne le premier chemin executable trouve
> (ici `/bin/ls`). Si elle contenait un `/`, on testerait directement `access(X_OK)`."

Montre la logique dans `path.c` :

```
grep -n "strchr" path.c
grep -n "access" path.c
```

---

## Etape 7 — Pas de fork si commande introuvable (tache 0.3)

```
echo "qwerty_nexiste_pas" | ./hsh
echo $?
```

Resultat attendu :

```
./hsh: 1: qwerty_nexiste_pas: not found
127
```

**A dire :**
> "Avant de forker, on appelle `find_command`. Si elle retourne `NULL`,
> on affiche l'erreur et on retourne `127` SANS faire de fork. C'est une
> exigence explicite de la tache 0.3 — pas de fork inutile."

Preuve dans le code :

```
grep -n "find_command" execute.c
```

Tu vois que `find_command(args[0])` est appele AVANT `fork()`.

---

## Etape 8 — Format d'erreur conforme a /bin/sh

Comparaison en live :

```
echo "qwerty" | ./hsh
echo "qwerty" | /bin/sh
```

**A dire :**
> "Format identique a `/bin/sh` : `nom_programme: numero_ligne: commande: not found`.
> La seule difference autorisee est `argv[0]` (./hsh vs /bin/sh)."

---

## Etape 9 — Built-in `exit` (tache 0.4)

```
echo "exit" | ./hsh
echo $?
```

Status `0`. Puis en interactif :

```
./hsh
$ exit
```

Le shell se termine.

**A dire :**
> "`exit` est un built-in : il doit terminer le processus shell lui-meme,
> donc on ne peut pas le deleguer a un binaire externe — sinon on quitterait
> juste l'enfant, pas le shell parent. Il est traite AVANT la resolution `PATH`."

---

## Etape 10 — Built-in `env` (tache 1.0)

```
echo "env" | ./hsh | head -5
```

**A dire :**
> "`env` parcourt la variable globale `environ` (declaree `extern` par la libc)
> et affiche chaque entree sur une ligne. Comme `exit`, c'est un built-in
> pour pouvoir acceder directement a l'environnement du shell."

---

## Etape 11 — Comparaison bit a bit avec /bin/sh

Batterie de tests a taper en live :

```
echo "/bin/echo OK" | ./hsh     ;  echo "/bin/echo OK" | /bin/sh
echo "ls" | ./hsh | head -3     ;  echo "ls" | /bin/sh | head -3
echo "ls -l /tmp" | ./hsh       ;  echo "ls -l /tmp" | /bin/sh
echo "qwerty" | ./hsh           ;  echo "qwerty" | /bin/sh
echo "exit" | ./hsh ; echo $?   ;  echo "exit" | /bin/sh ; echo $?
```

**A dire :**
> "Meme sortie standard, meme sortie erreur (a `argv[0]` pres), meme code de sortie."

---

## Etape 12 — Pas de fuite memoire

```
valgrind --leak-check=full --show-leak-kinds=all ./hsh <<< "/bin/ls"
```

Tu dois voir :

```
All heap blocks were freed -- no leaks are possible
```

**A dire :**
> "Toutes les allocations sont liberees : `line` par `free` a la fin de `shell_loop`,
> `args` par `free_args`, le chemin resolu par `free(cmd_path)` dans `execute_cmd`."

---

## Etape 13 — Tour du code (2 minutes max)

Ouvre les fichiers dans l'ordre :

```
cat shell.h
cat main.c
cat shell.c
cat parser.c
cat path.c
cat execute.c
cat builtins.c
```

**Pour chaque fichier, dis en une phrase :**

- `shell.h` : prototypes + includes + `extern char **environ`.
- `main.c` : point d'entree, delegue tout a `shell_loop(av[0])`.
- `shell.c` : la boucle REPL — prompt, `getline`, parse, dispatch, loop.
- `parser.c` : tokenisation avec `strtok(" \t")`, tableau `NULL`-termine.
- `path.c` : `find_command` — direct si `/`, sinon boucle sur `PATH`.
- `execute.c` : `fork` + `execve` + `waitpid`, `127` si introuvable.
- `builtins.c` : dispatch de `exit` et `env`.

---

## Etape 14 — Questions pieges probables

**Q : Pourquoi `write(STDOUT_FILENO, "$ ", 2)` et pas `printf("$ ")` ?**
R : `write` est un syscall direct, sans bufferisation. Le prompt s'affiche
immediatement. `printf` pourrait rester dans le buffer stdio et s'afficher
apres la saisie.

**Q : Pourquoi `_exit(127)` et pas `exit(127)` dans l'enfant ?**
R : `exit` flush les buffers stdio, qui ont ete dupliques par `fork`.
Ca peut provoquer un double flush. `_exit` termine immediatement sans
toucher aux buffers.

**Q : Pourquoi `(void)ac;` dans `main` ?**
R : Le shell de base n'utilise pas `ac`, mais `-Wunused-parameter` ferait
echouer la compilation. Le cast vers `void` signale au compilateur "je sais".

**Q : Avez-vous des variables globales ?**
R : Non. `environ` est declaree `extern` — elle est fournie par la libc,
pas par nous. Notre code n'en definit aucune.

**Q : Comment gerez-vous Ctrl+D vs Ctrl+C ?**
R : Ctrl+D envoie EOF, `getline` retourne `-1`, on `break`. Ctrl+C n'est pas
gere dans cette version (tasks 0-6 ne l'exigent pas) : le shell quitterait.
On pourrait ajouter un handler `SIGINT` si necessaire dans une version avancee.

**Q : Pourquoi `strdup` dans `count_tokens` ?**
R : `strtok` modifie sa chaine source en placant des `\0`. Si on appelait
`strtok` directement sur `line`, on ne pourrait plus retokeniser ensuite
dans `parse_line`. Donc on travaille sur une copie pour compter, puis sur
l'original pour extraire.

**Q : Et si `PATH` n'existe pas ?**
R : `get_path_value` retourne `NULL`, `find_command` retourne `NULL`,
donc la commande est consideree introuvable (exit `127`).

**Q : Difference entre fonction et syscall ?**
R : Une fonction tourne en user space (`strlen`, `printf`, `getline`).
Un syscall fait un appel au noyau pour un service OS (`fork`, `execve`,
`waitpid`, `read`, `write`, `access`).

---

## Checklist express avant de rentrer en review

- [ ] Binaire `hsh` absent avant la demo (compile live)
- [ ] Terminal propre, curseur pret
- [ ] `README.md`, `AUTHORS`, `man_1_simple_shell` presents
- [ ] `DOC_NOTIONS_FR.md` et `PREPA_MAN_REVIEW_FR.md` relus
- [ ] Tu connais la place de chaque fonction (`grep -n <fonction> *.c`)
- [ ] Tu sais expliquer pourquoi on ne forke pas si commande introuvable
- [ ] Tu sais expliquer ce qu'est un built-in et pourquoi `exit` en est un

Bonne review !
