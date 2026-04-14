# simple_shell (refait)

Interpreteur de commandes UNIX minimal (`hsh`) refait dans un dossier separe, en se basant sur le repo fourni et les consignes Holberton.

## Fonctionnalites

- Mode interactif (affiche `($) `) et non-interactif
- Lecture de ligne avec `getline`
- Parsing simple par espaces et tabulations
- Recherche dans `PATH`
- Execution avec `fork` + `execve`
- Attente du processus fils avec `waitpid`
- Built-ins: `exit`, `env`
- Message `not found` au format attendu:
  - `argv[0]: line: command: not found`

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

## Utilisation

### Interactif

```bash
./hsh
($) /bin/ls
($) env
($) exit
```

### Non interactif

```bash
echo "/bin/ls" | ./hsh
```

## Fichiers

- `main.c`: point d'entree
- `shell.c`: boucle principale
- `parser.c`: tokenisation de la ligne
- `path.c`: resolution des commandes via `PATH`
- `execute.c`: fork/exec/wait + erreurs
- `builtins.c`: `exit` et `env`
- `shell.h`: prototypes et includes
- `man_1_simple_shell`: page de manuel
- `AUTHORS`: auteurs
