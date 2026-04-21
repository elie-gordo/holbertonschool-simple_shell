#include "shell.h"

/**
 * count_tokens - Count the number of tokens in a line
 * @line: input string to analyze
 *
 * Return: number of space/tab-separated tokens, 0 on failure
 *
 * Description: Makes a copy of the line because strtok modifies
 * the string it operates on. Counts words delimited by spaces/tabs.
 */
static int count_tokens(char *line)
{
	int count = 0;   /* Token counter */
	char *copy;      /* Working copy of the line */
	char *token;     /* Current token from strtok */

	copy = strdup(line); /* strtok is destructive: work on a copy */
	if (copy == NULL)
		return (0);
	token = strtok(copy, " \t"); /* Get first token */
	while (token != NULL)
	{
		count++;                       /* Count this token */
		token = strtok(NULL, " \t");   /* Get next token */
	}
	free(copy); /* Free the working copy */
	return (count);
}

/**
 * parse_line - Split a line into a NULL-terminated argv array
 * @line: input line (will be modified by strtok)
 *
 * Return: allocated array of token pointers, or NULL if empty
 *
 * Description: Allocates the argv container only. Tokens point
 * directly into the line buffer (no extra strdup per token).
 * The array is NULL-terminated as required by execve.
 */
char **parse_line(char *line)
{
	int total;       /* Number of tokens found */
	int i = 0;       /* Index into the args array */
	char **args;     /* The argv array to return */
	char *token;     /* Current token from strtok */

	total = count_tokens(line); /* Count tokens first */
	if (total == 0)
		return (NULL); /* No tokens: empty line */
	args = malloc(sizeof(char *) * (total + 1)); /* +1 for NULL */
	if (args == NULL)
		return (NULL);
	token = strtok(line, " \t"); /* Tokenize the original line */
	while (token != NULL)
	{
		args[i] = token; /* Store pointer into line buffer */
		i++;
		token = strtok(NULL, " \t"); /* Next token */
	}
	args[i] = NULL; /* NULL-terminate for execve */
	return (args);
}

/**
 * free_args - Free the argv array container
 * @args: array allocated by parse_line
 *
 * Description: Only frees the array pointer, not the individual
 * strings, because they point into the line buffer managed by getline.
 */
void free_args(char **args)
{
	free(args);
}
