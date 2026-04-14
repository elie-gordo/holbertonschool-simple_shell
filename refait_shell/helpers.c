#include "shell.h"

/**
 * parse_input - Split one input line into argv-like tokens.
 * @line: Mutable buffer returned by getline.
 *
 * Return: Allocated token array or NULL when the line is empty.
 */
char **parse_input(char *line)
{
	char **tokens;
	char *token;
	int i;

	tokens = malloc(sizeof(char *) * MAX_ARGS);
	if (tokens == NULL)
		return (NULL);
	i = 0;
	token = strtok(line, " \t\n");
	while (token != NULL && i < MAX_ARGS - 1)
	{
		tokens[i] = token;
		i++;
		token = strtok(NULL, " \t\n");
	}
	tokens[i] = NULL;
	if (i == 0)
	{
		free(tokens);
		return (NULL);
	}
	return (tokens);
}

/**
 * free_tokens - Free token container returned by parse_input.
 * @tokens: Token array.
 */
void free_tokens(char **tokens)
{
	free(tokens);
}
