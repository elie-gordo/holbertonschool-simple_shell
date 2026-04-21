# simple_shell

Interpreteur de commandes UNIX minimal ecrit en C (projet Holberton).

## 1) Objectif du projet

Construire un shell capable de:
- lire une commande depuis stdin,
- la parser,
- chercher un binaire executable,
- creer un processus enfant,
- executer la commande avec execve,
- attendre la fin de l'enfant,
- recommencer.

Ce depot couvre les tasks 0 a 6, c'est-a-dire jusqu'a la version 1.0:
- support des arguments,
- recherche dans PATH,
- built-ins exit et env.

## 2) Compilation

Commande imposee:

gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

## 3) Utilisation

Mode interactif:

./hsh
#cisfun$ /bin/ls
#cisfun$ ls -l
#cisfun$ env
#cisfun$ exit

Mode non interactif:

echo "/bin/ls" | ./hsh

## 4) Fonctionnalites par task

### Task 0 - README, man, AUTHORS
- README present
- page man presente: man_1_simple_shell
- fichier AUTHORS present

### Task 1 - Betty
- code structure en fichiers courts
- commentaires de fonctions presents
- indentation claire et uniforme

### Task 2 - Simple shell 0.1
- prompt en interactif
- lecture de ligne avec getline
- execution d'une commande simple
- gestion EOF (Ctrl+D)
- gestion des erreurs

### Task 3 - Simple shell 0.2
- gestion des arguments (ex: ls -l /tmp)

### Task 4 - Simple shell 0.3
- recherche de la commande dans PATH
- pas de fork si commande introuvable

### Task 5 - Simple shell 0.4
- built-in exit

### Task 6 - Simple shell 1.0
- built-in env

## 5) Architecture des fichiers

- shell.h: includes, prototypes, organisation globale
- main.c: point d'entree
- shell.c: boucle principale
- parser.c: decoupage de la ligne en tokens
- path.c: resolution d'une commande via PATH
- execute.c: fork/execve/waitpid + gestion status
- builtins.c: built-ins exit et env
- man_1_simple_shell: page de manuel
- AUTHORS: contributeurs

## 6) Notions techniques a maitriser

### Histoire
- UNIX originel: Ken Thompson et Dennis Ritchie
- Premier shell UNIX: Ken Thompson
- Langage B: Ken Thompson

### Processus
- pid: identifiant du processus courant
- ppid: identifiant du parent
- fork: duplique le processus
- execve: remplace l'image du processus courant
- waitpid: attend la fin d'un enfant

### Environnement
- envp/environ: tableau NAME=VALUE
- PATH: liste de repertoires ou chercher les commandes

### Parsing
- strtok sur espaces/tabulations
- tableau argv termine par NULL pour execve

### Erreurs
- commande introuvable: "prog: line: cmd: not found"
- erreurs systeme: perror

## 7) Limites volontaires (scope du projet)

Non implemente:
- pipes
- redirections
- operateurs ; && ||
- guillemets avances
- variables shell personnalisees

## 8) Exemples de tests utiles

Commande valide:

echo "/bin/echo OK" | ./hsh

Commande via PATH:

echo "ls" | ./hsh

Commande introuvable:

echo "qwerty" | ./hsh

Built-in env:

echo "env" | ./hsh

Sortie propre:

echo "exit" | ./hsh

## 9) Ressources

- man sh
- man fork
- man execve
- man waitpid
- man getline
- man environ

## 10) Auteurs

Voir le fichier AUTHORS.

- Elie <elliott.lanza@gmail.com>
- toxicgiosue <toxicgiosue@gmail.com>
