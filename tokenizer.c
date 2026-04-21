#include "shell.h"

/**
 * _count_words - Count the number of words in a string
 * @line: input string to analyze
 *
 * Return: number of space/tab-separated words
 *
 * Description: Handles single quotes so that quoted strings
 * with spaces count as one word (needed for alias values).
 */
static int _count_words(char *line)
{
	int count = 0, in_word = 0, in_q = 0, i = 0;

	while (line[i])
	{
		if (line[i] == '\'')
			in_q = !in_q; /* Toggle quote state */
		if ((line[i] != ' ' && line[i] != '\t') || in_q)
		{
			if (!in_word) /* Start of a new word */
			{
				count++;
				in_word = 1;
			}
		}
		else
			in_word = 0; /* Inside a delimiter */
		i++;
	}
	return (count);
}

/**
 * _get_word - Extract the next word from a string
 * @line: input string
 * @start: pointer to current position (updated after extraction)
 *
 * Return: newly allocated word string, or NULL on failure
 *
 * Description: Skips leading whitespace, then copies characters
 * until the next unquoted space/tab. Handles single quotes.
 */
static char *_get_word(char *line, int *start)
{
	int end, len, in_q = 0;
	char *word;

	while (line[*start] == ' ' || line[*start] == '\t')
		(*start)++; /* Skip leading whitespace */
	if (line[*start] == '\0')
		return (NULL); /* No more words */
	end = *start;
	while (line[end]) /* Find end of word */
	{
		if (line[end] == '\'')
			in_q = !in_q; /* Toggle quote state */
		else if ((line[end] == ' ' || line[end] == '\t') && !in_q)
			break; /* Unquoted delimiter: end of word */
		end++;
	}
	len = end - *start; /* Calculate word length */
	word = malloc(len + 1); /* Allocate space for word + null */
	if (word == NULL)
		return (NULL);
	memcpy(word, line + *start, len); /* Copy word characters */
	word[len] = '\0'; /* Null-terminate */
	*start = end; /* Move position past this word */
	return (word);
}

/**
 * tokenize - Split a string into an array of word tokens
 * @line: input string to tokenize
 *
 * Return: NULL-terminated array of token strings, or NULL if empty
 *
 * Description: Custom replacement for strtok. Each token is a
 * separately allocated string. Caller must use free_tokens().
 */
char **tokenize(char *line)
{
	int count, i, pos = 0;
	char **tokens;

	count = _count_words(line); /* How many tokens? */
	if (count == 0)
		return (NULL); /* Empty line */
	tokens = malloc(sizeof(char *) * (count + 1)); /* +1 for NULL */
	if (tokens == NULL)
		return (NULL);
	for (i = 0; i < count; i++)
	{
		tokens[i] = _get_word(line, &pos); /* Extract each token */
		if (tokens[i] == NULL) /* Allocation failed mid-way */
		{
			while (--i >= 0)
				free(tokens[i]); /* Rollback */
			free(tokens);
			return (NULL);
		}
	}
	tokens[count] = NULL; /* NULL-terminate for execve */
	return (tokens);
}

/**
 * free_tokens - Free a token array and all its strings
 * @tokens: array returned by tokenize()
 */
void free_tokens(char **tokens)
{
	int i;

	if (tokens == NULL)
		return;
	for (i = 0; tokens[i]; i++)
		free(tokens[i]); /* Free each individual token */
	free(tokens); /* Free the array itself */
}
