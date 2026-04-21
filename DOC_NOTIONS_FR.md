# Documentation des notions - simple_shell

Ce document regroupe les notions theoriques a maitriser pour expliquer le projet.

## 1) Comment fonctionne un shell

Un shell suit une boucle:
1. afficher un prompt (si interactif)
2. lire une ligne
3. parser la ligne
4. executer la commande
5. attendre la fin
6. recommencer

## 2) Fonction vs system call

- Fonction: code de bibliotheque en espace utilisateur (ex: strtok, printf).
- System call: passage en noyau pour demander un service OS (ex: fork, execve, waitpid).

## 3) PID et PPID

- PID: identifiant unique du processus courant.
- PPID: PID du parent qui a cree ce processus.

Dans ce projet:
- le shell parent cree un enfant avec fork;
- l'enfant execute la commande;
- le parent attend.

## 4) execve

execve remplace le code du processus courant par le binaire cible.

Signature:
int execve(const char *path, char *const argv[], char *const envp[]);

Points importants:
- ne retourne pas en cas de succes;
- retourne -1 en cas d'erreur;
- argv doit finir par NULL.

## 5) fork + waitpid

fork:
- retourne -1 en erreur;
- retourne 0 dans l'enfant;
- retourne PID enfant dans le parent.

waitpid:
- permet au parent de recuperer le statut enfant;
- WIFEXITED(status) pour sortie normale;
- WEXITSTATUS(status) pour code de sortie.

## 6) PATH

PATH est une variable env du type:
/usr/local/bin:/usr/bin:/bin

Strategie:
- si la commande contient '/', on teste directement access(X_OK);
- sinon on parcourt chaque repertoire de PATH et on construit dir/commande.

## 7) Parsing

Le projet utilise strtok avec delimiteurs espace et tabulation.

Exemple:
"ls -l /tmp" devient:
- args[0] = "ls"
- args[1] = "-l"
- args[2] = "/tmp"
- args[3] = NULL

## 8) Environnement

envp contient les variables d'environnement heritees.

Built-in env:
- affiche simplement chaque entree de envp sur une ligne.

## 9) EOF (Ctrl+D)

getline retourne -1 a EOF.

Comportement attendu:
- quitter proprement la boucle;
- liberer les allocations;
- afficher un retour ligne en interactif pour une sortie propre.

## 10) Erreurs courantes a eviter

- oublier de liberer line alloue par getline;
- fork alors que la commande n'existe pas;
- oublier NULL final dans argv;
- confondre status brut de waitpid et code de sortie reel;
- parser une ligne vide comme commande executable.

## 11) Contraintes Holberton a rappeler

- compilation en gnu89 avec flags stricts;
- 5 fonctions max par fichier;
- include guards dans les headers;
- pas de fuites memoire;
- style Betty.
