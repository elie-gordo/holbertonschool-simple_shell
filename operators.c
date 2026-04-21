#include "shell.h"

/**
 * _find_op - Find the next operator (; && ||) in a string
 * @line: input string to scan
 * @start: position to start searching from
 * @pos: pointer to store the position of the found operator
 *
 * Return: 1 for ;, 2 for &&, 3 for ||, 0 if no operator found
 */
static int _find_op(char *line, int start, int *pos)
{
	int i = start;

	while (line[i])
	{
		if (line[i] == ';') /* Semicolon: sequential execution */
		{
			*pos = i;
			return (1);
		}
		if (line[i] == '&' && line[i + 1] == '&') /* AND operator */
		{
			*pos = i;
			return (2);
		}
		if (line[i] == '|' && line[i + 1] == '|') /* OR operator */
		{
			*pos = i;
			return (3);
		}
		i++;
	}
	return (0); /* No operator found */
}

/**
 * _build_alias_line - Replace first word with alias value
 * @alias_val: the alias value to substitute
 * @seg: original segment string
 *
 * Return: newly allocated expanded line, or NULL on failure
 *
 * Description: Finds the end of the first word in seg, then
 * builds a new string: alias_val + rest_of_seg.
 */
static char *_build_alias_line(char *alias_val, char *seg)
{
	int i = 0;
	char *result;
	int alen, slen;

	while (seg[i] && seg[i] != ' ' && seg[i] != '\t')
		i++; /* Skip past the first word (alias name) */
	alen = strlen(alias_val); /* Length of alias value */
	slen = strlen(seg + i); /* Length of remaining arguments */
	result = malloc(alen + slen + 1); /* Allocate combined string */
	if (result == NULL)
		return (NULL);
	memcpy(result, alias_val, alen); /* Copy alias value */
	memcpy(result + alen, seg + i, slen); /* Append remaining args */
	result[alen + slen] = '\0'; /* Null-terminate */
	return (result);
}

/**
 * exec_segment - Execute a single command segment
 * @sh: pointer to shell state
 * @seg: command string to execute (one segment between operators)
 *
 * Description: Expands variables, checks for alias, tokenizes,
 * then dispatches to builtin or external command execution.
 */
void exec_segment(shell_t *sh, char *seg)
{
	char *expanded, *alias_val, *new_line;
	char **args;

	expanded = expand_variables(sh, seg); /* Replace $? $$ $VAR */
	if (expanded == NULL || expanded[0] == '\0')
	{
		free(expanded);
		return; /* Nothing to execute */
	}
	args = tokenize(expanded); /* Split into argv array */
	if (args == NULL)
	{
		free(expanded);
		return; /* Empty after tokenization */
	}
	alias_val = get_alias_value(sh, args[0]); /* Check alias */
	if (alias_val) /* First word is an alias: expand it */
	{
		new_line = _build_alias_line(alias_val, expanded);
		free(expanded);
		free_tokens(args);
		if (new_line == NULL)
			return;
		args = tokenize(new_line); /* Re-tokenize with alias value */
		free(new_line);
		if (args == NULL)
			return;
	}
	else
		free(expanded); /* No alias: free expanded string */
	if (run_builtin(args, sh) == 0) /* Try builtin first */
		sh->status = execute_cmd(args, sh); /* Else external cmd */
	free_tokens(args); /* Free the argument array */
}

/**
 * handle_operators - Split a line by ; && || and execute each segment
 * @sh: pointer to shell state
 * @line: full input line (may contain operators)
 *
 * Description: Scans for operators left to right. For each segment:
 * ;  -> always execute next segment
 * && -> execute next only if previous succeeded (status == 0)
 * || -> execute next only if previous failed (status != 0)
 */
void handle_operators(shell_t *sh, char *line)
{
	int start = 0, pos = 0, op, len;
	int run = 1; /* Whether to execute the current segment */
	char save;   /* Saved char for temporary null-termination */

	while (1)
	{
		op = _find_op(line, start, &pos); /* Find next operator */
		if (op == 0) /* No more operators: execute remainder */
		{
			if (run)
				exec_segment(sh, line + start);
			break;
		}
		save = line[pos];  /* Save char at operator position */
		line[pos] = '\0';  /* Temporarily terminate segment */
		if (run)
			exec_segment(sh, line + start); /* Execute segment */
		line[pos] = save;  /* Restore original char */
		len = (op == 1) ? 1 : 2; /* ; is 1 char, && || are 2 */
		start = pos + len; /* Move past the operator */
		if (op == 1)
			run = 1; /* ; : always run next */
		else if (op == 2)
			run = (sh->status == 0); /* && : run if success */
		else
			run = (sh->status != 0); /* || : run if failure */
		if (sh->should_exit)
			break; /* exit was called: stop processing */
	}
}
