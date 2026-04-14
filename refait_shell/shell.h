#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 64

extern char **environ;

char **parse_input(char *line);
void free_tokens(char **tokens);

char *get_env_value(const char *name);
char *resolve_command_path(char *command);

int handle_builtin(char **args, char *prog_name, unsigned int line_no,
		int *exit_status, int *should_exit);
int execute_line(char **args, char *prog_name, unsigned int line_no,
		int *exit_status, int *should_exit);
int shell_loop(char *prog_name);

void print_environment(void);
void print_not_found(char *prog_name, unsigned int line_no, char *cmd);

#endif
