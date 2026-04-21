#include "shell.h"

/**
 * _find_alias_idx - Find the index of an alias by name
 * @sh: shell state
 * @name: alias name to search for
 *
 * Return: index in sh->aliases if found, -1 if not found
 */
static int _find_alias_idx(shell_t *sh, char *name)
{
	int i, nlen;

	nlen = strlen(name); /* Length of the name to match */
	for (i = 0; i < sh->alias_count; i++)
	{
		/* Check if alias starts with "name=" */
		if (strncmp(sh->aliases[i], name, nlen) == 0
			&& sh->aliases[i][nlen] == '=')
			return (i); /* Found at index i */
	}
	return (-1); /* Not found */
}

/**
 * get_alias_value - Get the value part of an alias
 * @sh: shell state
 * @name: alias name to look up
 *
 * Return: pointer to the value string, or NULL if not found
 *
 * Description: Strips surrounding single quotes from the value
 * if present (e.g. "ll='ls -la'" returns "ls -la").
 */
char *get_alias_value(shell_t *sh, char *name)
{
	int idx, nlen, vlen;
	char *val;

	idx = _find_alias_idx(sh, name);
	if (idx == -1)
		return (NULL); /* Alias does not exist */
	nlen = strlen(name);
	val = sh->aliases[idx] + nlen + 1; /* Skip past "name=" */
	if (*val == '\'') /* Value is quoted: strip quotes */
	{
		val++; /* Skip opening quote */
		vlen = strlen(val);
		if (vlen > 0 && val[vlen - 1] == '\'')
			val[vlen - 1] = '\0'; /* Remove closing quote */
	}
	return (val);
}

/**
 * _set_alias - Create or update an alias definition
 * @sh: shell state
 * @def: full alias definition string (e.g. "name='value'")
 *
 * Return: 0 on success, -1 on failure
 */
static int _set_alias(shell_t *sh, char *def)
{
	char *eq, *name;
	int idx, nlen;

	eq = strchr(def, '='); /* Find the = separator */
	if (eq == NULL)
		return (-1); /* Invalid format: no = sign */
	nlen = eq - def; /* Length of the alias name */
	name = malloc(nlen + 1);
	if (name == NULL)
		return (-1);
	memcpy(name, def, nlen); /* Extract the name part */
	name[nlen] = '\0';
	idx = _find_alias_idx(sh, name); /* Check if alias exists */
	free(name);
	if (idx >= 0) /* Alias exists: replace its value */
	{
		free(sh->aliases[idx]);
		sh->aliases[idx] = strdup(def);
		return (0);
	}
	if (sh->alias_count >= MAX_ALIASES) /* Array is full */
		return (-1);
	sh->aliases[sh->alias_count] = strdup(def); /* Add new alias */
	sh->alias_count++;
	return (0);
}

/**
 * builtin_alias - Handle the alias builtin command
 * @args: argument vector
 * @sh: shell state
 *
 * Return: 1 (handled)
 *
 * Description: With no args, prints all aliases. With name=value,
 * defines aliases. With just names, prints those specific aliases.
 */
int builtin_alias(char **args, shell_t *sh)
{
	int i, idx;

	if (args[1] == NULL) /* No args: print all aliases */
	{
		for (i = 0; i < sh->alias_count; i++)
			printf("%s\n", sh->aliases[i]);
		sh->status = 0;
		return (1);
	}
	for (i = 1; args[i]; i++) /* Process each argument */
	{
		if (strchr(args[i], '=') != NULL)
			_set_alias(sh, args[i]); /* Has =: define alias */
		else
		{
			idx = _find_alias_idx(sh, args[i]);
			if (idx >= 0)
				printf("%s\n", sh->aliases[idx]); /* Print it */
			else
				fprintf(stderr, "%s: %d: alias: %s not found\n",
					sh->name, sh->count, args[i]);
		}
	}
	sh->status = 0;
	return (1);
}
