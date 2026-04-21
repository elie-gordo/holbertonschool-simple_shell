#include "shell.h"

/**
 * print_env - Print all environment variables to stdout
 *
 * Description: Iterates through the global environ array
 * and prints each "NAME=value" entry on its own line.
 */
static void print_env(void)
{
	int i = 0;

	while (environ[i] != NULL)
	{
		printf("%s\n", environ[i]); /* Print each variable */
		i++;
	}
}

/**
 * handle_builtin - Check if a command is a builtin and execute it
 * @args: parsed argument vector (args[0] = command name)
 * @should_exit: pointer to flag, set to 1 if exit is called
 *
 * Return: 1 if the command was a builtin (handled), 0 otherwise
 *
 * Description: Supports two builtins:
 * - "exit": sets should_exit flag to break the main loop
 * - "env": prints all environment variables
 * Returns 0 for non-builtins so the caller can try execute_cmd.
 */
int handle_builtin(char **args, int *should_exit)
{
	/* Check for exit builtin */
	if (strcmp(args[0], "exit") == 0)
	{
		*should_exit = 1; /* Signal the loop to stop */
		return (1); /* Handled */
	}
	/* Check for env builtin */
	if (strcmp(args[0], "env") == 0)
	{
		print_env(); /* Print all environment variables */
		return (1); /* Handled */
	}
	return (0); /* Not a builtin */
}
