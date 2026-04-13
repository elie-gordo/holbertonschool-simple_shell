#ifndef SHELL_H
#define SHELL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int simple_shell(char *prog_name, char **envp);
char **parse_line(char *line);
void free_args(char **args);
int execute_command(char **args, char *prog_name, char **envp, int cmd_count);

#endif
