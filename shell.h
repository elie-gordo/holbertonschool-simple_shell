#ifndef SHELL_H
#define SHELL_H

/* printf, fprintf, perror */
#include <stdio.h>
/* malloc, free, exit */
#include <stdlib.h>
/* strlen, strcmp, strncmp, strdup, strchr, strtok */
#include <string.h>
/* read, write, access, isatty, fork, execve */
#include <unistd.h>
/* pid_t */
#include <sys/types.h>
/* waitpid, WIFEXITED, WEXITSTATUS */
#include <sys/wait.h>

/* Global environment inherited from the system */
extern char **environ;

/* shell.c - Main REPL loop */
int shell_loop(char *prog_name);

/* parser.c - Tokenize input line into argv array */
char **parse_line(char *line);
void free_args(char **args);

/* path.c - Resolve command name to executable path */
char *find_command(char *cmd);

/* execute.c - Fork and execute external commands */
int execute_cmd(char **args, char *prog_name, int cmd_count);
void print_not_found(char *prog_name, int cmd_count, char *cmd);

/* builtins.c - Handle exit and env built-in commands */
int handle_builtin(char **args, int *should_exit);

#endif
