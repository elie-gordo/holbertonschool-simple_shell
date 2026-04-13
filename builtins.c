#include "shell.h"

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
	(void)envp;

	if (strcmp(args[0], "exit") == 0)
	{
		*should_exit = 1;
		return (1);
	}
	return (0);
}
