#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#define BUF_SIZE 1024
#define MAX_ALIASES 128
#define HIST_MAX 4096

extern char **environ;
extern volatile sig_atomic_t g_signal;

/**
 * struct shell_s - Shell state
 * @name: program name (argv[0])
 * @count: command line counter
 * @status: last exit status
 * @interactive: 1 if interactive mode
 * @should_exit: flag to break the loop
 * @env: dynamic environment copy
 * @env_count: number of env variables
 * @aliases: alias strings in name='value' format
 * @alias_count: number of aliases
 * @hist: history entries
 * @hist_count: number of history entries
 */
typedef struct shell_s
{
	char *name;
	int count;
	int status;
	int interactive;
	int should_exit;
	char **env;
	int env_count;
	char *aliases[MAX_ALIASES];
	int alias_count;
	char **hist;
	int hist_count;
} shell_t;

/* main.c */
void sigint_handler(int sig);

/* shell_loop.c */
int shell_loop(shell_t *sh);
int file_mode(shell_t *sh, char *filename);

/* input.c */
ssize_t _getline(char **lineptr, size_t *n, int fd);

/* tokenizer.c */
char **tokenize(char *line);
void free_tokens(char **tokens);

/* operators.c */
void handle_operators(shell_t *sh, char *line);
void exec_segment(shell_t *sh, char *seg);

/* variables.c */
char *expand_variables(shell_t *sh, char *line);
void strip_comments(char *line);

/* path.c */
char *find_command(char *cmd, shell_t *sh);

/* execute.c */
int execute_cmd(char **args, shell_t *sh);
void print_not_found(shell_t *sh, char *cmd);

/* builtins.c */
int run_builtin(char **args, shell_t *sh);

/* builtins2.c */
int builtin_setenv(char **args, shell_t *sh);
int builtin_unsetenv(char **args, shell_t *sh);
int builtin_cd(char **args, shell_t *sh);

/* builtins3.c */
int builtin_alias(char **args, shell_t *sh);
char *get_alias_value(shell_t *sh, char *name);

/* builtins4.c */
int builtin_help(char **args, shell_t *sh);
int builtin_history(shell_t *sh);

/* environ.c */
int copy_environ(shell_t *sh);
char *_getenv(shell_t *sh, char *name);
int _setenv(shell_t *sh, char *name, char *value);
int _unsetenv(shell_t *sh, char *name);
void free_environ(shell_t *sh);

/* history.c */
int load_history(shell_t *sh);
int save_history(shell_t *sh);
int add_history(shell_t *sh, char *line);
void free_history(shell_t *sh);

/* helpers.c */
int _atoi_custom(char *s);
char *_itoa(int n);
int _is_number(char *s);

#endif
