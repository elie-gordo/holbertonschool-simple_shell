#include "shell.h"

/**
 * strip_comments - Remove everything after a # comment marker
 * @line: input string (modified in place)
 *
 * Description: A # starts a comment only if it is at position 0
 * or preceded by a space or tab character.
 */
void strip_comments(char *line)
{
	int i = 0;

	if (line[0] == '#') /* Comment at start of line */
	{
		line[0] = '\0';
		return;
	}
	while (line[i])
	{
		if (line[i] == '#' && i > 0
			&& (line[i - 1] == ' ' || line[i - 1] == '\t'))
		{
			line[i] = '\0'; /* Truncate at comment start */
			return;
		}
		i++;
	}
}

/**
 * _append - Append a string to a dynamically growing result buffer
 * @res: pointer to result buffer (may be reallocated)
 * @rlen: pointer to current length of content in result
 * @rcap: pointer to total capacity of result buffer
 * @src: string to append (may be NULL, in which case nothing happens)
 */
static void _append(char **res, int *rlen, int *rcap, char *src)
{
	int slen;

	if (src == NULL)
		return; /* Nothing to append */
	slen = strlen(src);
	while (*rlen + slen + 1 > *rcap) /* Grow buffer if needed */
	{
		*rcap *= 2;
		*res = realloc(*res, *rcap);
	}
	memcpy(*res + *rlen, src, slen); /* Copy source into result */
	*rlen += slen;
	(*res)[*rlen] = '\0'; /* Keep null-terminated */
}

/**
 * _expand_dollar - Expand a single $ variable at current position
 * @sh: shell state (for status, pid, env lookup)
 * @line: input string being scanned
 * @i: pointer to current index (positioned after $, updated on return)
 * @res: pointer to result buffer
 * @rlen: pointer to result length
 * @rcap: pointer to result capacity
 *
 * Description: Handles $? (exit status), $$ (PID), $VAR (env var).
 * If $ is followed by an unknown char, the literal $ is kept.
 */
static void _expand_dollar(shell_t *sh, char *line, int *i,
	char **res, int *rlen, int *rcap)
{
	char *val, name[256];
	int j = 0;

	(*i)++; /* Move past the $ character */
	if (line[*i] == '?') /* $? : last exit status */
	{
		val = _itoa(sh->status);
		_append(res, rlen, rcap, val);
		free(val);
		(*i)++;
	}
	else if (line[*i] == '$') /* $$ : current process ID */
	{
		val = _itoa((int)getpid());
		_append(res, rlen, rcap, val);
		free(val);
		(*i)++;
	}
	else if (line[*i] == '_' || (line[*i] >= 'A' && line[*i] <= 'Z')
		|| (line[*i] >= 'a' && line[*i] <= 'z'))
	{ /* $VARIABLE : environment variable lookup */
		while (line[*i] && (line[*i] == '_'
			|| (line[*i] >= 'A' && line[*i] <= 'Z')
			|| (line[*i] >= 'a' && line[*i] <= 'z')
			|| (line[*i] >= '0' && line[*i] <= '9')))
			name[j++] = line[(*i)++]; /* Collect variable name */
		name[j] = '\0';
		val = _getenv(sh, name); /* Look up in environment */
		_append(res, rlen, rcap, val); /* Append value (or nothing) */
	}
	else /* $ followed by non-variable char: keep literal $ */
	{
		char dollar[2] = {'$', '\0'};

		_append(res, rlen, rcap, dollar);
	}
}

/**
 * expand_variables - Expand all $?, $$, and $VAR in a line
 * @sh: shell state
 * @line: input string to expand
 *
 * Return: newly allocated string with all variables expanded
 *
 * Description: Scans the line character by character. When a $
 * is found followed by a valid variable reference, it is replaced
 * with the corresponding value. All other characters are copied as-is.
 */
char *expand_variables(shell_t *sh, char *line)
{
	char *res;
	int i = 0, rlen = 0, rcap;
	char c[2] = {'\0', '\0'}; /* Single char buffer for _append */

	rcap = strlen(line) * 2 + 64; /* Initial capacity with margin */
	res = malloc(rcap);
	if (res == NULL)
		return (strdup(line)); /* Fallback: return unmodified copy */
	res[0] = '\0';
	while (line[i])
	{
		if (line[i] == '$' && line[i + 1])
			_expand_dollar(sh, line, &i, &res, &rlen, &rcap);
		else
		{
			c[0] = line[i++]; /* Copy literal character */
			_append(&res, &rlen, &rcap, c);
		}
	}
	return (res);
}
