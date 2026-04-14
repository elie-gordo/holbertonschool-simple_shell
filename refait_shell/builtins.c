#include "shell.h"

/**
 * print_environment - Print each environment variable on one line.
 */
void print_environment(void)
{
	int i;

	i = 0;
	while (environ[i] != NULL)
	{
		printf("%s\n", environ[i]);
		i++;
	}
}

/**
 * parse_exit_value - Convert unsigned decimal string to exit code.
 * @s: String to parse.
 *
 * Return: Parsed value in [0,255], or -1 if invalid.
 */
static int parse_exit_value(char *s)
{
	int i;
	unsigned long value;

	if (s == NULL || s[0] == '\0')
		return (-1);
	i = 0;
	value = 0;
	while (s[i] != '\0')
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		value = (value * 10) + (s[i] - '0');
		if (value > 4294967295UL)
			return (-1);
		i++;
	}
	return ((int)(value % 256UL));
}

/**
 * handle_builtin - Execute builtin command if present.
 * @args: Parsed command tokens.
 * @prog_name: Program name for error messages.
 * @line_no: Current command line number.
 * @exit_status: Last shell status.
 * @should_exit: Set to 1 when shell must stop.
 *
 * Return: 1 if builtin handled, 0 otherwise.
 */
int handle_builtin(char **args, char *prog_name, unsigned int line_no,
		int *exit_status, int *should_exit)
{
	int code;

	if (args == NULL || args[0] == NULL)
		return (0);
	if (strcmp(args[0], "exit") == 0)
	{
		if (args[1] != NULL)
		{
			code = parse_exit_value(args[1]);
			if (code == -1)
			{
				fprintf(stderr, "%s: %u: exit: Illegal number: %s\n",
					prog_name, line_no, args[1]);
				*exit_status = 2;
				*should_exit = 1;
				return (1);
			}
			*exit_status = code;
		}
		*should_exit = 1;
		return (1);
	}
	if (strcmp(args[0], "env") == 0)
	{
		print_environment();
		*exit_status = 0;
		return (1);
	}
	return (0);
}
