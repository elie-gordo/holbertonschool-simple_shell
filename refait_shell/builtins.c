#include "shell.h"

/**
 * print_env - Print environment variables.
 * @envp: Environment array.
 */
static void print_env(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
	{
		printf("%s\n", envp[i]);
		i++;
	}
}

/**
 * shell_atoi - Parse a positive integer string.
 * @s: String to parse.
 *
 * Return: Parsed int, or -1 on invalid input.
 */
int shell_atoi(char *s)
{
	int i;
	unsigned int n;

	if (s == NULL || s[0] == '\0')
		return (-1);
	i = 0;
	n = 0;
	while (s[i] != '\0')
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		n = (n * 10) + (s[i] - '0');
		i++;
	}
	return ((int)(n % 256));
}

/**
 * handle_builtin - Handle built-in commands.
 * @args: Parsed args.
 * @envp: Environment variables.
 * @should_exit: Exit flag output.
 * @exit_status: Shell exit status output.
 *
 * Return: 1 if built-in, 0 otherwise.
 */
int handle_builtin(char **args, char **envp, int *should_exit, int *exit_status)
{
	int code;

	if (strcmp(args[0], "exit") == 0)
	{
		if (args[1] != NULL)
		{
			code = shell_atoi(args[1]);
			if (code != -1)
				*exit_status = code;
		}
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
