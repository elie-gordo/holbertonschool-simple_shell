#include "shell.h"

/**
 * copy_environ - Copy the global environ into a dynamic array
 * @sh: shell state
 *
 * Return: 0 on success, -1 on failure
 *
 * Description: Creates a deep copy of environ so we can modify
 * it freely with setenv/unsetenv and pass it to execve.
 */
int copy_environ(shell_t *sh)
{
	int i, count = 0;

	while (environ[count])
		count++; /* Count number of environment variables */
	sh->env = malloc(sizeof(char *) * (count + 1)); /* +1 for NULL */
	if (sh->env == NULL)
		return (-1);
	for (i = 0; i < count; i++)
	{
		sh->env[i] = strdup(environ[i]); /* Duplicate each entry */
		if (sh->env[i] == NULL) /* Allocation failed: rollback */
		{
			while (--i >= 0)
				free(sh->env[i]);
			free(sh->env);
			sh->env = NULL;
			return (-1);
		}
	}
	sh->env[count] = NULL; /* NULL-terminate the array */
	sh->env_count = count;
	return (0);
}

/**
 * _getenv - Look up an environment variable by name
 * @sh: shell state
 * @name: variable name to find (e.g. "PATH")
 *
 * Return: pointer to the value part, or NULL if not found
 */
char *_getenv(shell_t *sh, char *name)
{
	int i, nlen;

	if (name == NULL || sh->env == NULL)
		return (NULL);
	nlen = strlen(name);
	for (i = 0; i < sh->env_count; i++)
	{
		/* Match "NAME=" prefix exactly */
		if (strncmp(sh->env[i], name, nlen) == 0
			&& sh->env[i][nlen] == '=')
			return (sh->env[i] + nlen + 1); /* Return value part */
	}
	return (NULL); /* Variable not found */
}

/**
 * _setenv - Set or update an environment variable
 * @sh: shell state
 * @name: variable name
 * @value: variable value
 *
 * Return: 0 on success, -1 on failure
 *
 * Description: If the variable exists, replaces its value.
 * If not, appends a new entry and grows the array with realloc.
 */
int _setenv(shell_t *sh, char *name, char *value)
{
	int i, nlen, len;
	char *entry, **tmp;

	nlen = strlen(name);
	len = nlen + strlen(value) + 2; /* name + = + value + \0 */
	entry = malloc(len);
	if (entry == NULL)
		return (-1);
	sprintf(entry, "%s=%s", name, value); /* Build "NAME=value" */
	for (i = 0; i < sh->env_count; i++) /* Search for existing */
	{
		if (strncmp(sh->env[i], name, nlen) == 0
			&& sh->env[i][nlen] == '=')
		{
			free(sh->env[i]); /* Free old entry */
			sh->env[i] = entry; /* Replace with new one */
			return (0);
		}
	}
	/* Variable not found: add new entry */
	tmp = realloc(sh->env, sizeof(char *) * (sh->env_count + 2));
	if (tmp == NULL)
	{
		free(entry);
		return (-1);
	}
	sh->env = tmp;
	sh->env[sh->env_count] = entry; /* Add at end */
	sh->env_count++;
	sh->env[sh->env_count] = NULL; /* Keep NULL-terminated */
	return (0);
}

/**
 * _unsetenv - Remove an environment variable from the array
 * @sh: shell state
 * @name: variable name to remove
 *
 * Return: 0 on success, -1 if variable not found
 */
int _unsetenv(shell_t *sh, char *name)
{
	int i, nlen;

	nlen = strlen(name);
	for (i = 0; i < sh->env_count; i++)
	{
		if (strncmp(sh->env[i], name, nlen) == 0
			&& sh->env[i][nlen] == '=')
		{
			free(sh->env[i]); /* Free the entry */
			/* Move last entry to fill the gap */
			sh->env[i] = sh->env[sh->env_count - 1];
			sh->env_count--;
			sh->env[sh->env_count] = NULL; /* Keep terminated */
			return (0);
		}
	}
	return (-1); /* Variable not found */
}
