#include "shell.h"

/**
 * print_env - Prints current environment, one variable per line.
 * @envp: Environment array.
 */
static void print_env(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
	{
		write(STDOUT_FILENO, envp[i], strlen(envp[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}

/**
 * handle_builtin - Handles supported built-ins.
 * @args: Parsed command arguments.
 * @envp: Environment array.
 * @should_exit: Exit flag set to 1 for built-in exit.
 *
 * Return: 1 if a built-in was handled, 0 otherwise.
 */
int handle_builtin(char **args, char **envp, int *should_exit)
{
	if (strcmp(args[0], "exit") == 0)
	{
		*should_exit = 1;
		return (1);
	}
	if (strcmp(args[0], "env") == 0)
	{
		print_env(envp);
		return (1);
	}
	return (0);
}
