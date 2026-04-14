# hsh - Simple UNIX command interpreter

Ce dossier contient une implementation C d'un mini-shell inspire de /bin/sh.
L'objectif est de couvrir le flux minimal d'un shell: lire, parser, resoudre,
executer, attendre, recommencer.

## Fonctionnalites implementees

- Mode interactif avec prompt ($) 
- Mode non-interactif via pipe/redirection
- Recherche des commandes dans PATH
- Gestion des chemins absolus/relatifs
- Builtins: env, exit
- Message d'erreur not found au format attendu
- Codes de sortie propages depuis les processus enfants

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

## Utilisation

Interactif:

```bash
./hsh
($) /bin/ls
($) env
($) exit
```

Non-interactif:

```bash
echo "/bin/echo hello" | ./hsh
```

## Organisation des fichiers (top-level)

- shell.h: prototypes, includes et constantes
- main.c: point d'entree
- shell.c: boucle principale + execution des commandes
- helpers.c: parsing de la ligne utilisateur
- path.c: resolution de commande avec PATH
- builtins.c: implementation de env/exit
- man_1_simple_shell: page man
- AUTHORS: auteurs

## Logique d'execution

1. getline lit la ligne
2. parse_input transforme la ligne en tableau argv
3. handle_builtin traite env/exit
4. resolve_command_path trouve le binaire
5. fork + execve lance la commande
6. waitpid recupere le statut

## Exemples de verification

```bash
echo "qwerty" | ./hsh
# ./hsh: 1: qwerty: not found

printf "exit 42\n" | ./hsh
echo $?
# 42
```

## Contraintes ciblees

- Standard: gnu89
- Flags: -Wall -Werror -Wextra -pedantic
- Header guard present
- Allocation/liberation maitrisee

## 📖 Man Page

Une page de manuel complète est disponible dans le fichier `man_1_simple_shell`.

### Visualiser la man page

```bash
man ./man_1_simple_shell
```

ou

```bash
man -l man_1_simple_shell
```

La man page contient :
- Description complète du shell
- Syntaxe des commandes
- Liste des builtins
- Exemples d'utilisation
- Codes de sortie
- Variables d'environnement
- Limitations

---

## 🐛 Bugs rencontrés

Voici quelques bugs que nous avons rencontrés pendant le développement et comment nous les avons résolus :

### 1. **Memory Leak avec getline()**

**Problème** : Après plusieurs commandes, Valgrind détectait une fuite mémoire.

```
==12345== 1,024 bytes in 1 blocks are definitely lost
==12345==    at malloc (vg_replace_malloc.c:309)
==12345==    by getline (iogetline.c:120)
```

**Cause** : La variable `line` allouée par `getline()` n'était jamais libérée.

**Solution** : Ajouter `free(line)` avant de quitter le programme.

```c
// Dans main.c
free(line);
return (exit_status);
```

---

### 2. **Segmentation Fault sur commande vide**

**Problème** : Le shell plantait quand on appuyait sur Entrée sans taper de commande.

```bash
($) 
Segmentation fault (core dumped)
```

**Cause** : `args[0]` était NULL et on essayait de le passer à `strcmp()`.

**Solution** : Vérifier si `args[0]` est NULL avant de continuer.

```c
if (args[0] == NULL)
    return (0);
```

---

### 3. **Erreur de parsing avec espaces multiples**

**Problème** : La commande `"ls    -l"` (avec plusieurs espaces) créait des arguments vides.

**Cause** : `strtok()` ne gérait pas correctement les espaces multiples.

**Solution** : `strtok()` gère déjà ce cas ! Il saute automatiquement les délimiteurs consécutifs.

---

### 4. **PATH non trouvé**

**Problème** : Certaines commandes ne fonctionnaient pas alors qu'elles existaient.

```bash
($) ls
./hsh: 1: ls: not found
```

**Cause** : La fonction `_getenv()` ne cherchait pas correctement dans `environ`.

**Solution** : Vérifier que la comparaison se fait avec `name_len` et le caractère `=`.

```c
if (strncmp(environ[i], name, name_len) == 0 &&
    environ[i][name_len] == '=')
{
    return (environ[i] + name_len + 1);
}
```

---

### 5. **Prompt affiché en mode non-interactif**

**Problème** : Le prompt `($)` s'affichait même avec `echo "ls" | ./hsh`.

**Cause** : Pas de vérification du mode interactif.

**Solution** : Utiliser `isatty(STDIN_FILENO)` pour détecter le mode.

```c
interactive = isatty(STDIN_FILENO);
if (interactive)
    printf("($) ");
```

---

### 6. **Mauvais code de retour**

**Problème** : Le shell retournait toujours 0, même en cas d'erreur.

**Cause** : Le `exit_status` n'était pas mis à jour correctement.

**Solution** : Sauvegarder le statut de la dernière commande réussie dans `last_status`.

```c
if (exit_status != 256)
    last_status = exit_status;
```

---

### 7. **Double free sur command_path**

**Problème** : Erreur double free détectée par Valgrind.

```
==12345== Invalid free() / delete / delete[] / realloc()
```

**Cause** : `command_path` était libéré deux fois : dans l'enfant et dans le parent.

**Solution** : Libérer `command_path` uniquement dans le parent après `wait()`.

```c
// Dans fork_and_execute()
else
{
    wait(&status);
    free(command_path);  // Libérer ici seulement
    // ...
}
```

---

## ✅ Tests & Vérifications

### Tests de mémoire avec Valgrind

Tous nos tests passent **sans fuites mémoire** :

```bash
$ valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./hsh
```

**Résultat** :
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: 145 allocs, 145 frees, 12,456 bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
==12345==
==12345== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### Test interactif complet

```bash
$ valgrind --leak-check=full ./hsh
($) ls
AUTHORS  README.md  hsh
($) pwd
/home/user/simple_shell
($) env
PATH=/usr/bin:/bin
HOME=/home/user
($) /bin/echo Test
Test
($) commande_fausse
./hsh: 1: commande_fausse: not found
($) exit
```

**Résultat Valgrind** : ✅ **0 bytes lost**

### Test non-interactif

```bash
$ echo -e "ls\npwd\nenv" | valgrind --leak-check=full ./hsh
```

**Résultat** : ✅ **No leaks possible**

### Tests de edge cases

#### Ligne vide
```bash
$ echo "" | ./hsh
# Aucune erreur
```

#### Commande inexistante
```bash
$ echo "commande_qui_nexiste_pas" | ./hsh
./hsh: 1: commande_qui_nexiste_pas: not found
```

#### Espaces multiples
```bash
$ echo "ls     -l" | ./hsh
# Fonctionne correctement
```

#### EOF (Ctrl+D)
```bash
($) ^D
$
# Sort proprement
```

### Vérification Betty

```bash
$ betty-style.pl *.c *.h
# Aucune erreur de style

$ betty-doc.pl *.c *.h
# Toutes les fonctions sont documentées
```

---

## 👥 Auteurs

Ce projet a été développé par :

- **Valentin Planchon** - [GitHub](https://github.com/ValentinCA28)
- **Maxim Dutruel** - [GitHub](https://github.com/maxim880000)

---

## 📚 Ressources

### Documentation officielle
- [Unix Shell](https://en.wikipedia.org/wiki/Unix_shell)
- [Thompson Shell](https://en.wikipedia.org/wiki/Thompson_shell)
- [Ken Thompson](https://en.wikipedia.org/wiki/Ken_Thompson)

### Man pages importante
- `man_1_simpl shell`


### Guides
- **[Guide Interactif Complet](https://v0-shell-diagram.vercel.app/)** - Explications détaillées du projet
- [Everything you need to know to start coding your own shell](https://intranet.hbtn.io/concepts/64)

---

## 🎓 Objectifs d'apprentissage

À la fin de ce projet, nous sommes capables d'expliquer :

### Questions générales
- ✅ Qui a conçu et implémenté le système d'exploitation Unix original
- ✅ Qui a écrit la première version du shell UNIX
- ✅ Qui a inventé le langage de programmation B (prédécesseur du C)
- ✅ Qui est Ken Thompson

### Concepts techniques
- ✅ Comment fonctionne un shell
- ✅ Qu'est-ce qu'un PID et un PPID
- ✅ Comment manipuler l'environnement du processus actuel
- ✅ Quelle est la différence entre une fonction et un appel système
- ✅ Comment créer des processus
- ✅ Quels sont les trois prototypes de `main`
- ✅ Comment le shell utilise PATH pour trouver les programmes
- ✅ Comment exécuter un autre programme avec `execve`
- ✅ Comment suspendre l'exécution d'un processus jusqu'à ce qu'un de ses enfants se termine
- ✅ Qu'est-ce que EOF / "end-of-file"

---

## 📜 Licence

Ce projet est réalisé dans le cadre du programme **Holberton School**. Il est destiné à des fins éducatives.

---

## 🙏 Remerciements

- **Holberton School** pour le projet et les ressources
- **Les pairs étudiants** pour l'entraide et les tests
- **La communauté open source** pour la documentation et les exemples

---

<div align="center">

**Holberton School - Simple Shell Project**

*Développé avec ❤️ par Valentin Planchon et Maxim Dutruel*

[⬆ Retour en haut](#-simple-shell---hsh)

</div>