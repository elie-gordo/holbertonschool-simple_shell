#include "shell.h"

/**
 * copy_environ - Copy the global environ into a dynamic array
 * @sh: shell state
 *
 * Return: 0 on success, -1 on failure
 */
int copy_environ(shell_t *sh)
{
	int i, count = 0;

	while (environ[count])
		count++;
	sh->env = malloc(sizeof(char *) * (count + 1));
	if (sh->env == NULL)
		return (-1);
	for (i = 0; i < count; i++)
	{
		sh->env[i] = strdup(environ[i]);
		if (sh->env[i] == NULL)
		{
			while (--i >= 0)
				free(sh->env[i]);
			free(sh->env);
			sh->env = NULL;
			return (-1);
		}
	}
	sh->env[count] = NULL;
	sh->env_count = count;
	return (0);
}

/**
 * _getenv - Get the value of an environment variable
 * @sh: shell state
 * @name: variable name
 *
 * Return: pointer to the value, or NULL if not found
 */
char *_getenv(shell_t *sh, char *name)
{
	int i, nlen;

	if (name == NULL || sh->env == NULL)
		return (NULL);
	nlen = strlen(name);
	for (i = 0; i < sh->env_count; i++)
	{
		if (strncmp(sh->env[i], name, nlen) == 0
			&& sh->env[i][nlen] == '=')
			return (sh->env[i] + nlen + 1);
	}
	return (NULL);
}

/**
 * _setenv - Set or update an environment variable
 * @sh: shell state
 * @name: variable name
 * @value: variable value
 *
 * Return: 0 on success, -1 on failure
 */
int _setenv(shell_t *sh, char *name, char *value)
{
	int i, nlen, len;
	char *entry, **tmp;

	nlen = strlen(name);
	len = nlen + strlen(value) + 2;
	entry = malloc(len);
	if (entry == NULL)
		return (-1);
	sprintf(entry, "%s=%s", name, value);
	for (i = 0; i < sh->env_count; i++)
	{
		if (strncmp(sh->env[i], name, nlen) == 0
			&& sh->env[i][nlen] == '=')
		{
			free(sh->env[i]);
			sh->env[i] = entry;
			return (0);
		}
	}
	tmp = realloc(sh->env, sizeof(char *) * (sh->env_count + 2));
	if (tmp == NULL)
	{
		free(entry);
		return (-1);
	}
	sh->env = tmp;
	sh->env[sh->env_count] = entry;
	sh->env_count++;
	sh->env[sh->env_count] = NULL;
	return (0);
}

/**
 * _unsetenv - Remove an environment variable
 * @sh: shell state
 * @name: variable name to remove
 *
 * Return: 0 on success, -1 if not found
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
			free(sh->env[i]);
			sh->env[i] = sh->env[sh->env_count - 1];
			sh->env_count--;
			sh->env[sh->env_count] = NULL;
			return (0);
		}
	}
	return (-1);
}

/**
 * free_environ - Free the dynamic environment array
 * @sh: shell state
 */
void free_environ(shell_t *sh)
{
	int i;

	if (sh->env == NULL)
		return;
	for (i = 0; i < sh->env_count; i++)
		free(sh->env[i]);
	free(sh->env);
	sh->env = NULL;
	sh->env_count = 0;
}
