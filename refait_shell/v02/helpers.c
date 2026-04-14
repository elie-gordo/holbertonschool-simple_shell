#include "shell.h"

/**
 * parse_line - Splits a line into arguments
 * @line: The line to split
 *
 * Return: Array of arguments
 */
char **parse_line(char *line)
{
	char **args = malloc(sizeof(char *) * MAX_ARGS);
	char *token;
	int i = 0;

	if (!args)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " \t\n");
	while (token != NULL && i < MAX_ARGS - 1)
	{
		args[i++] = token;
		token = strtok(NULL, " \t\n");
	}
	args[i] = NULL;

	return (args);
}

/**
 * free_args - Frees the array of arguments
 * @args: Array to free
 */
void free_args(char **args)
{
	free(args);
}
