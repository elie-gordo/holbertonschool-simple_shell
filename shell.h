#ifndef SHELL_H
#define SHELL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int simple_shell(char *prog_name, char **envp);
char **parse_line(char *line);
void free_args(char **args);
char *find_command(char *cmd, char **envp);
int execute_command(char **args, char *prog_name, char **envp, int cmd_count);
int handle_builtin(char **args, char **envp, int *should_exit);
void print_not_found(char *prog_name, int cmd_count, char *cmd);

#endif
