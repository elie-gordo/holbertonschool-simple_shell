#include "shell.h"

/**
 * strip_comments - Remove everything after a # comment marker
 * @line: input string (modified in place)
 *
 * Description: A # is a comment start only at the beginning
 * or after a space/tab.
 */
void strip_comments(char *line)
{
	int i = 0;

	if (line[0] == '#')
	{
		line[0] = '\0';
		return;
	}
	while (line[i])
	{
		if (line[i] == '#' && i > 0
			&& (line[i - 1] == ' ' || line[i - 1] == '\t'))
		{
			line[i] = '\0';
			return;
		}
		i++;
	}
}

/**
 * _append - Append a string to result buffer, growing if needed
 * @res: pointer to result buffer
 * @rlen: pointer to current length
 * @rcap: pointer to capacity
 * @src: string to append
 */
static void _append(char **res, int *rlen, int *rcap, char *src)
{
	int slen;

	if (src == NULL)
		return;
	slen = strlen(src);
	while (*rlen + slen + 1 > *rcap)
	{
		*rcap *= 2;
		*res = realloc(*res, *rcap);
	}
	memcpy(*res + *rlen, src, slen);
	*rlen += slen;
	(*res)[*rlen] = '\0';
}

/**
 * _expand_dollar - Expand a $ variable at position i in line
 * @sh: shell state
 * @line: input string
 * @i: pointer to position (updated past the variable)
 * @res: pointer to result buffer
 * @rlen: pointer to result length
 * @rcap: pointer to result capacity
 */
static void _expand_dollar(shell_t *sh, char *line, int *i,
	char **res, int *rlen, int *rcap)
{
	char *val, name[256];
	int j = 0;

	(*i)++;
	if (line[*i] == '?')
	{
		val = _itoa(sh->status);
		_append(res, rlen, rcap, val);
		free(val);
		(*i)++;
	}
	else if (line[*i] == '$')
	{
		val = _itoa((int)getpid());
		_append(res, rlen, rcap, val);
		free(val);
		(*i)++;
	}
	else if (line[*i] == '_' || (line[*i] >= 'A' && line[*i] <= 'Z')
		|| (line[*i] >= 'a' && line[*i] <= 'z'))
	{
		while (line[*i] && (line[*i] == '_'
			|| (line[*i] >= 'A' && line[*i] <= 'Z')
			|| (line[*i] >= 'a' && line[*i] <= 'z')
			|| (line[*i] >= '0' && line[*i] <= '9')))
			name[j++] = line[(*i)++];
		name[j] = '\0';
		val = _getenv(sh, name);
		_append(res, rlen, rcap, val);
	}
	else
	{
		char dollar[2] = {'$', '\0'};

		_append(res, rlen, rcap, dollar);
	}
}

/**
 * expand_variables - Expand $?, $$, and $VAR in a line
 * @sh: shell state
 * @line: input string
 *
 * Return: newly allocated expanded string
 */
char *expand_variables(shell_t *sh, char *line)
{
	char *res;
	int i = 0, rlen = 0, rcap;
	char c[2] = {'\0', '\0'};

	rcap = strlen(line) * 2 + 64;
	res = malloc(rcap);
	if (res == NULL)
		return (strdup(line));
	res[0] = '\0';
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1])
			_expand_dollar(sh, line, &i, &res, &rlen, &rcap);
		else
		{
			c[0] = line[i++];
			_append(&res, &rlen, &rcap, c);
		}
	}
	return (res);
}
