#include "shell.h"

/**
 * count_tokens - Count space/tab separated tokens.
 * @line: Raw input line.
 *
 * Return: Number of tokens.
 */
static int count_tokens(char *line)
{
	int count;
	char *copy;
	char *token;

	copy = strdup(line);
	if (copy == NULL)
		return (0);
	count = 0;
	token = strtok(copy, " \t");
	while (token != NULL)
	{
		count++;
		token = strtok(NULL, " \t");
	}
	free(copy);
	return (count);
}

/**
 * parse_line - Tokenize a command line.
 * @line: Raw input line.
 *
 * Return: Array of args (NULL terminated), or NULL.
 */
char **parse_line(char *line)
{
	int total;
	int i;
	char **args;
	char *token;

	total = count_tokens(line);
	if (total == 0)
		return (NULL);
	args = malloc(sizeof(char *) * (total + 1));
	if (args == NULL)
		return (NULL);
	token = strtok(line, " \t");
	i = 0;
	while (token != NULL)
	{
		args[i] = token;
		i++;
		token = strtok(NULL, " \t");
	}
	args[i] = NULL;
	return (args);
}

/**
 * free_args - Free args array container.
 * @args: Token array.
 */
void free_args(char **args)
{
	free(args);
}
