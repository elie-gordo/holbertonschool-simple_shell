#include "shell.h"

/**
 * _print_help_all - Print general help listing all builtins
 */
static void _print_help_all(void)
{
	printf("Simple Shell built-in commands:\n");
	printf("  exit [status]    - Exit the shell\n");
	printf("  env              - Print environment\n");
	printf("  setenv VAR VAL   - Set environment variable\n");
	printf("  unsetenv VAR     - Remove environment variable\n");
	printf("  cd [dir]         - Change directory\n");
	printf("  alias [name=val] - Manage aliases\n");
	printf("  help [builtin]   - Display help\n");
	printf("  history          - Display command history\n");
}

/**
 * _print_help_one - Print help for a specific builtin
 * @name: builtin name
 *
 * Return: 0 if found, -1 if not a builtin
 */
static int _print_help_one(char *name)
{
	if (strcmp(name, "exit") == 0)
		printf("exit: exit [status]\n    Exit the shell.\n");
	else if (strcmp(name, "env") == 0)
		printf("env: env\n    Print the environment.\n");
	else if (strcmp(name, "setenv") == 0)
		printf("setenv: setenv VARIABLE VALUE\n    Set env variable.\n");
	else if (strcmp(name, "unsetenv") == 0)
		printf("unsetenv: unsetenv VARIABLE\n    Remove env variable.\n");
	else if (strcmp(name, "cd") == 0)
		printf("cd: cd [dir]\n    Change the current directory.\n");
	else if (strcmp(name, "alias") == 0)
		printf("alias: alias [name[='value'] ...]\n    Manage aliases.\n");
	else if (strcmp(name, "help") == 0)
		printf("help: help [BUILTIN]\n    Display help information.\n");
	else if (strcmp(name, "history") == 0)
		printf("history: history\n    Display command history.\n");
	else
		return (-1);
	return (0);
}

/**
 * builtin_help - Handle the help builtin command
 * @args: argument vector
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
int builtin_help(char **args, shell_t *sh)
{
	if (args[1] == NULL)
		_print_help_all();
	else
	{
		if (_print_help_one(args[1]) == -1)
		{
			fprintf(stderr, "%s: %d: help: no help topics match '%s'\n",
				sh->name, sh->count, args[1]);
			sh->status = 1;
			return (1);
		}
	}
	sh->status = 0;
	return (1);
}

/**
 * builtin_history - Display command history
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
int builtin_history(shell_t *sh)
{
	int i;

	for (i = 0; i < sh->hist_count; i++)
		printf("%5d  %s\n", i, sh->hist[i]);
	sh->status = 0;
	return (1);
}
