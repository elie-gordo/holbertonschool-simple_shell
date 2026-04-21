#ifndef SHELL_H
#define SHELL_H

/* Standard I/O: printf, fprintf, sprintf, perror */
#include <stdio.h>
/* Memory: malloc, realloc, free, exit */
#include <stdlib.h>
/* Strings: strlen, strcmp, strncmp, strdup, strchr, strcpy, memset, memcpy */
#include <string.h>
/* POSIX: read, write, access, chdir, getcwd, isatty, fork, execve, getpid */
#include <unistd.h>
/* Types: pid_t, size_t, ssize_t */
#include <sys/types.h>
/* Process: waitpid, WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG */
#include <sys/wait.h>
/* File info: stat, struct stat */
#include <sys/stat.h>
/* Signals: signal, SIGINT, SIG_DFL */
#include <signal.h>
/* File control: open, O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC */
#include <fcntl.h>
/* Error codes: errno */
#include <errno.h>
/* Limits: PATH_MAX */
#include <limits.h>

/* Size of the static read buffer for _getline */
#define BUF_SIZE 1024
/* Maximum number of aliases the shell can store */
#define MAX_ALIASES 128
/* Maximum number of history entries */
#define HIST_MAX 4096

/* Global environment variable from the system */
extern char **environ;

/**
 * struct shell_s - Shell state structure holding all runtime data
 * @name: program name (argv[0]), used in error messages
 * @count: command line counter, incremented for each input line
 * @status: exit status of the last executed command
 * @interactive: 1 if stdin is a terminal, 0 otherwise
 * @should_exit: flag set by exit builtin to break the main loop
 * @env: dynamic copy of the environment (modifiable)
 * @env_count: number of variables in the env array
 * @aliases: array of alias strings in "name='value'" format
 * @alias_count: number of defined aliases
 * @hist: array of history entry strings
 * @hist_count: number of entries in the history
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

/* main.c - Entry point and signal handling */
void sigint_handler(int sig);

/* shell_loop.c - Main REPL loop and file input mode */
int shell_loop(shell_t *sh);
int file_mode(shell_t *sh, char *filename);

/* input.c - Custom getline using buffered read() */
ssize_t _getline(char **lineptr, size_t *n, int fd);

/* tokenizer.c - Custom tokenizer without strtok */
char **tokenize(char *line);
void free_tokens(char **tokens);

/* operators.c - Handling of ; && || operators */
void handle_operators(shell_t *sh, char *line);
void exec_segment(shell_t *sh, char *seg);

/* variables.c - $? $$ $VAR expansion and # comment stripping */
char *expand_variables(shell_t *sh, char *line);
void strip_comments(char *line);

/* path.c - Command resolution via PATH */
char *find_command(char *cmd, shell_t *sh);

/* execute.c - Fork/execve/waitpid process execution */
int execute_cmd(char **args, shell_t *sh);
void print_not_found(shell_t *sh, char *cmd);

/* builtins.c - Builtin dispatcher + exit + env */
int run_builtin(char **args, shell_t *sh);

/* builtins2.c - setenv, unsetenv, cd builtins */
int builtin_setenv(char **args, shell_t *sh);
int builtin_unsetenv(char **args, shell_t *sh);
int builtin_cd(char **args, shell_t *sh);

/* builtins3.c - alias builtin and alias lookup */
int builtin_alias(char **args, shell_t *sh);
char *get_alias_value(shell_t *sh, char *name);

/* builtins4.c - help and history builtins */
int builtin_help(char **args, shell_t *sh);
int builtin_history(shell_t *sh);

/* environ.c - Dynamic environment management */
int copy_environ(shell_t *sh);
char *_getenv(shell_t *sh, char *name);
int _setenv(shell_t *sh, char *name, char *value);
int _unsetenv(shell_t *sh, char *name);
void free_environ(shell_t *sh);

/* history.c - Persistent command history */
int load_history(shell_t *sh);
int save_history(shell_t *sh);
int add_history(shell_t *sh, char *line);
void free_history(shell_t *sh);

/* helpers.c - Number conversion utilities */
int _atoi_custom(char *s);
char *_itoa(int n);
int _is_number(char *s);

#endif
