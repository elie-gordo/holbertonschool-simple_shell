#ifndef SHELL_H
#define SHELL_H

/*
 * shell.h - Fichier d'en-tete principal du projet simple_shell.
 *
 * Ce header centralise :
 * - les includes systeme utilises par les differents modules ;
 * - les prototypes de fonctions ;
 * - la declaration de l'environnement global.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern char **environ;

/* shell.c */
int simple_shell(char *prog_name, char **envp);

/* parser.c */
char **parse_line(char *line);
void free_args(char **args);

/* path.c */
char *find_command(char *cmd, char **envp);

/* execute.c */
int execute_command(char **args, char *prog_name, char **envp, int cmd_count);

/* builtins.c */
int handle_builtin(char **args, char **envp, int *should_exit);

/* execute.c */
void print_not_found(char *prog_name, int cmd_count, char *cmd);

#endif
