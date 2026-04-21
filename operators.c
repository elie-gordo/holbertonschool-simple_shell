#include "shell.h"

/**
 * _find_op - Find the next operator (; && ||) in the line
 * @line: input string
 * @start: position to start searching from
 * @pos: pointer to store position of found operator
 *
 * Return: 1 for ;, 2 for &&, 3 for ||, 0 if none found
 */
static int _find_op(char *line, int start, int *pos)
{
	int i = start;

	while (line[i])
	{
		if (line[i] == ';')
		{
			*pos = i;
			return (1);
		}
		if (line[i] == '&' && line[i + 1] == '&')
		{
			*pos = i;
			return (2);
		}
		if (line[i] == '|' && line[i + 1] == '|')
		{
			*pos = i;
			return (3);
		}
		i++;
	}
	return (0);
}

/**
 * _build_alias_line - Build expanded line replacing first word with alias
 * @alias_val: the alias value
 * @seg: original segment string
 *
 * Return: newly allocated expanded line, or NULL
 */
static char *_build_alias_line(char *alias_val, char *seg)
{
	int i = 0;
	char *result;
	int alen, slen;

	while (seg[i] && seg[i] != ' ' && seg[i] != '\t')
		i++;
	alen = strlen(alias_val);
	slen = strlen(seg + i);
	result = malloc(alen + slen + 1);
	if (result == NULL)
		return (NULL);
	memcpy(result, alias_val, alen);
	memcpy(result + alen, seg + i, slen);
	result[alen + slen] = '\0';
	return (result);
}

/**
 * exec_segment - Execute a single command segment
 * @sh: pointer to shell state
 * @seg: command string to execute
 */
void exec_segment(shell_t *sh, char *seg)
{
	char *expanded, *alias_val, *new_line;
	char **args;

	expanded = expand_variables(sh, seg);
	if (expanded == NULL || expanded[0] == '\0')
	{
		free(expanded);
		return;
	}
	args = tokenize(expanded);
	if (args == NULL)
	{
		free(expanded);
		return;
	}
	alias_val = get_alias_value(sh, args[0]);
	if (alias_val)
	{
		new_line = _build_alias_line(alias_val, expanded);
		free(expanded);
		free_tokens(args);
		if (new_line == NULL)
			return;
		args = tokenize(new_line);
		free(new_line);
		if (args == NULL)
			return;
	}
	else
		free(expanded);
	if (run_builtin(args, sh) == 0)
		sh->status = execute_cmd(args, sh);
	free_tokens(args);
}

/**
 * handle_operators - Split line by ;, &&, || and execute segments
 * @sh: pointer to shell state
 * @line: full input line
 */
void handle_operators(shell_t *sh, char *line)
{
	int start = 0, pos = 0, op, len;
	int run = 1;
	char save;

	while (1)
	{
		op = _find_op(line, start, &pos);
		if (op == 0)
		{
			if (run)
				exec_segment(sh, line + start);
			break;
		}
		save = line[pos];
		line[pos] = '\0';
		if (run)
			exec_segment(sh, line + start);
		line[pos] = save;
		len = (op == 1) ? 1 : 2;
		start = pos + len;
		if (op == 1)
			run = 1;
		else if (op == 2)
			run = (sh->status == 0);
		else
			run = (sh->status != 0);
		if (sh->should_exit)
			break;
	}
}
