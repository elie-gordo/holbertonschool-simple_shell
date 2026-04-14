#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int simple_shell(char *prog_name, char **envp);
char **parse_line(char *line);
void free_args(char **args);
char *find_command(char *cmd, char **envp);
int execute_command(char **args, char *prog_name, char **envp, int cmd_count);
int handle_builtin(char **args, char **envp, int *should_exit, int *exit_status);
void print_not_found(char *prog_name, int cmd_count, char *cmd);
int shell_atoi(char *s);

#endif
