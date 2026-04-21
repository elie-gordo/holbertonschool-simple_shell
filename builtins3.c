#include "shell.h"

/**
 * _find_alias_idx - Find alias index by name
 * @sh: shell state
 * @name: alias name to find
 *
 * Return: index if found, -1 otherwise
 */
static int _find_alias_idx(shell_t *sh, char *name)
{
	int i, nlen;

	nlen = strlen(name);
	for (i = 0; i < sh->alias_count; i++)
	{
		if (strncmp(sh->aliases[i], name, nlen) == 0
			&& sh->aliases[i][nlen] == '=')
			return (i);
	}
	return (-1);
}

/**
 * get_alias_value - Get the value of an alias by name
 * @sh: shell state
 * @name: alias name
 *
 * Return: pointer to value (inside alias string), or NULL
 */
char *get_alias_value(shell_t *sh, char *name)
{
	int idx, nlen, vlen;
	char *val;

	idx = _find_alias_idx(sh, name);
	if (idx == -1)
		return (NULL);
	nlen = strlen(name);
	val = sh->aliases[idx] + nlen + 1;
	if (*val == '\'')
	{
		val++;
		vlen = strlen(val);
		if (vlen > 0 && val[vlen - 1] == '\'')
			val[vlen - 1] = '\0';
	}
	return (val);
}

/**
 * _set_alias - Set or update an alias
 * @sh: shell state
 * @def: alias definition string "name='value'"
 *
 * Return: 0 on success, -1 on failure
 */
static int _set_alias(shell_t *sh, char *def)
{
	char *eq, *name;
	int idx, nlen;

	eq = strchr(def, '=');
	if (eq == NULL)
		return (-1);
	nlen = eq - def;
	name = malloc(nlen + 1);
	if (name == NULL)
		return (-1);
	memcpy(name, def, nlen);
	name[nlen] = '\0';
	idx = _find_alias_idx(sh, name);
	free(name);
	if (idx >= 0)
	{
		free(sh->aliases[idx]);
		sh->aliases[idx] = strdup(def);
		return (0);
	}
	if (sh->alias_count >= MAX_ALIASES)
		return (-1);
	sh->aliases[sh->alias_count] = strdup(def);
	sh->alias_count++;
	return (0);
}

/**
 * builtin_alias - Handle the alias builtin command
 * @args: argument vector
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
int builtin_alias(char **args, shell_t *sh)
{
	int i, idx;

	if (args[1] == NULL)
	{
		for (i = 0; i < sh->alias_count; i++)
			printf("%s\n", sh->aliases[i]);
		sh->status = 0;
		return (1);
	}
	for (i = 1; args[i]; i++)
	{
		if (strchr(args[i], '=') != NULL)
			_set_alias(sh, args[i]);
		else
		{
			idx = _find_alias_idx(sh, args[i]);
			if (idx >= 0)
				printf("%s\n", sh->aliases[idx]);
			else
				fprintf(stderr, "%s: %d: alias: %s not found\n",
					sh->name, sh->count, args[i]);
		}
	}
	sh->status = 0;
	return (1);
}
