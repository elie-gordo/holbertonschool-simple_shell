#include "shell.h"

/**
 * builtin_exit - Handle the exit builtin command
 * @args: argument vector (exit [status])
 * @sh: shell state
 *
 * Return: 1 (always handled)
 *
 * Description: If a status argument is given, validates it as a
 * number. Sets should_exit flag to break the main loop.
 */
static int builtin_exit(char **args, shell_t *sh)
{
	int exit_status;

	if (args[1] != NULL) /* Status argument provided */
	{
		if (!_is_number(args[1])) /* Not a valid integer */
		{
			fprintf(stderr, "%s: %d: exit: Illegal number: %s\n",
				sh->name, sh->count, args[1]);
			sh->status = 2; /* Error code for bad argument */
			return (1); /* Don't exit on invalid number */
		}
		exit_status = _atoi_custom(args[1]); /* Convert to int */
		sh->status = exit_status; /* Set exit code */
	}
	sh->should_exit = 1; /* Signal the loop to stop */
	return (1);
}

/**
 * builtin_env - Print all environment variables to stdout
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
static int builtin_env(shell_t *sh)
{
	int i;

	for (i = 0; i < sh->env_count; i++)
		printf("%s\n", sh->env[i]); /* Print each NAME=value */
	sh->status = 0;
	return (1);
}

/**
 * _is_exit - Check if a command string is "exit"
 * @cmd: command string to check
 *
 * Return: 1 if cmd equals "exit", 0 otherwise
 */
static int _is_exit(char *cmd)
{
	return (strcmp(cmd, "exit") == 0);
}

/**
 * run_builtin - Dispatch a command to the appropriate builtin handler
 * @args: argument vector (args[0] is the command name)
 * @sh: shell state
 *
 * Return: 1 if a builtin was executed, 0 if args[0] is not a builtin
 *
 * Description: Checks args[0] against all known builtins. If a match
 * is found, calls the handler and returns 1. Otherwise returns 0
 * so the caller knows to try external command execution.
 */
int run_builtin(char **args, shell_t *sh)
{
	if (args == NULL || args[0] == NULL)
		return (0); /* No command to check */
	if (_is_exit(args[0]))
		return (builtin_exit(args, sh)); /* exit [status] */
	if (strcmp(args[0], "env") == 0)
		return (builtin_env(sh)); /* env */
	if (strcmp(args[0], "setenv") == 0)
		return (builtin_setenv(args, sh)); /* setenv VAR VALUE */
	if (strcmp(args[0], "unsetenv") == 0)
		return (builtin_unsetenv(args, sh)); /* unsetenv VAR */
	if (strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, sh)); /* cd [dir] */
	if (strcmp(args[0], "alias") == 0)
		return (builtin_alias(args, sh)); /* alias [name=value] */
	if (strcmp(args[0], "help") == 0)
		return (builtin_help(args, sh)); /* help [builtin] */
	if (strcmp(args[0], "history") == 0)
		return (builtin_history(sh)); /* history */
	return (0); /* Not a builtin */
}
