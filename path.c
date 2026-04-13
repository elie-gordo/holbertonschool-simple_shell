#include "shell.h"

/**
 * build_full_path - Concatenates directory and command into full path.
 * @dir: Directory path.
 * @cmd: Command name.
 *
 * Return: Newly allocated full path or NULL.
 */
static char *build_full_path(char *dir, char *cmd)
{
	int len;
	char *full;

	len = strlen(dir) + strlen(cmd) + 2;
	full = malloc(len);
	if (full == NULL)
		return (NULL);
	sprintf(full, "%s/%s", dir, cmd);
	return (full);
}

/**
 * get_path_value - Gets PATH value from envp array.
 * @envp: Environment array.
 *
 * Return: Pointer to PATH value, or NULL.
 */
static char *get_path_value(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_command - Resolves a command to an executable path.
 * @cmd: Command name or path.
 * @envp: Environment array.
 *
 * Return: Newly allocated executable path, or NULL if not found.
 */
char *find_command(char *cmd, char **envp)
{
	char *path_value;
	char *paths;
	char *dir;
	char *full;

	if (strchr(cmd, '/') != NULL)
		return (access(cmd, X_OK) == 0 ? strdup(cmd) : NULL);
	path_value = get_path_value(envp);
	if (path_value == NULL)
		return (NULL);
	paths = strdup(path_value);
	if (paths == NULL)
		return (NULL);
	dir = strtok(paths, ":");
	while (dir != NULL)
	{
		full = build_full_path(dir, cmd);
		if (full != NULL && access(full, X_OK) == 0)
		{
			free(paths);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}
	free(paths);
	return (NULL);
}
