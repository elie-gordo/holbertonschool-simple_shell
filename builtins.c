#include "shell.h"

/**
 * builtin_exit - Handle the exit builtin command
 * @args: argument vector
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
static int builtin_exit(char **args, shell_t *sh)
{
	int exit_status;

	if (args[1] != NULL)
	{
		if (!_is_number(args[1]))
		{
			fprintf(stderr, "%s: %d: exit: Illegal number: %s\n",
				sh->name, sh->count, args[1]);
			sh->status = 2;
			return (1);
		}
		exit_status = _atoi_custom(args[1]);
		sh->status = exit_status;
	}
	sh->should_exit = 1;
	return (1);
}

/**
 * builtin_env - Print all environment variables
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
static int builtin_env(shell_t *sh)
{
	int i;

	for (i = 0; i < sh->env_count; i++)
		printf("%s\n", sh->env[i]);
	sh->status = 0;
	return (1);
}

/**
 * _is_exit - Check if command is exit
 * @cmd: command string
 *
 * Return: 1 if match, 0 otherwise
 */
static int _is_exit(char *cmd)
{
	return (strcmp(cmd, "exit") == 0);
}

/**
 * run_builtin - Dispatch to the appropriate builtin handler
 * @args: argument vector
 * @sh: shell state
 *
 * Return: 1 if a builtin was handled, 0 if not a builtin
 */
int run_builtin(char **args, shell_t *sh)
{
	if (args == NULL || args[0] == NULL)
		return (0);
	if (_is_exit(args[0]))
		return (builtin_exit(args, sh));
	if (strcmp(args[0], "env") == 0)
		return (builtin_env(sh));
	if (strcmp(args[0], "setenv") == 0)
		return (builtin_setenv(args, sh));
	if (strcmp(args[0], "unsetenv") == 0)
		return (builtin_unsetenv(args, sh));
	if (strcmp(args[0], "cd") == 0)
		return (builtin_cd(args, sh));
	if (strcmp(args[0], "alias") == 0)
		return (builtin_alias(args, sh));
	if (strcmp(args[0], "help") == 0)
		return (builtin_help(args, sh));
	if (strcmp(args[0], "history") == 0)
		return (builtin_history(sh));
	return (0);
}
