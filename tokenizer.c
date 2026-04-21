#include "shell.h"

/**
 * _count_words - Count tokens separated by spaces and tabs
 * @line: input string
 *
 * Return: number of words
 */
static int _count_words(char *line)
{
	int count = 0, in_word = 0, in_q = 0, i = 0;

	while (line[i])
	{
		if (line[i] == '\'')
			in_q = !in_q;
		if ((line[i] != ' ' && line[i] != '\t') || in_q)
		{
			if (!in_word)
			{
				count++;
				in_word = 1;
			}
		}
		else
			in_word = 0;
		i++;
	}
	return (count);
}

/**
 * _get_word - Extract the next word from line starting at *start
 * @line: input string
 * @start: pointer to current position (updated after extraction)
 *
 * Return: newly allocated word, or NULL on failure
 */
static char *_get_word(char *line, int *start)
{
	int end, len, in_q = 0;
	char *word;

	while (line[*start] == ' ' || line[*start] == '\t')
		(*start)++;
	if (line[*start] == '\0')
		return (NULL);
	end = *start;
	while (line[end])
	{
		if (line[end] == '\'')
			in_q = !in_q;
		else if ((line[end] == ' ' || line[end] == '\t') && !in_q)
			break;
		end++;
	}
	len = end - *start;
	word = malloc(len + 1);
	if (word == NULL)
		return (NULL);
	memcpy(word, line + *start, len);
	word[len] = '\0';
	*start = end;
	return (word);
}

/**
 * tokenize - Split a line into an array of word tokens (no strtok)
 * @line: input string
 *
 * Return: NULL-terminated array of tokens, or NULL if empty/error
 */
char **tokenize(char *line)
{
	int count, i, pos = 0;
	char **tokens;

	count = _count_words(line);
	if (count == 0)
		return (NULL);
	tokens = malloc(sizeof(char *) * (count + 1));
	if (tokens == NULL)
		return (NULL);
	for (i = 0; i < count; i++)
	{
		tokens[i] = _get_word(line, &pos);
		if (tokens[i] == NULL)
		{
			while (--i >= 0)
				free(tokens[i]);
			free(tokens);
			return (NULL);
		}
	}
	tokens[count] = NULL;
	return (tokens);
}

/**
 * free_tokens - Free a token array
 * @tokens: array to free
 */
void free_tokens(char **tokens)
{
	int i;

	if (tokens == NULL)
		return;
	for (i = 0; tokens[i]; i++)
		free(tokens[i]);
	free(tokens);
}
