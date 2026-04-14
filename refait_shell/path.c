#include "shell.h"

/**
 * get_path_value - Read PATH value from envp.
 * @envp: Environment variables.
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
 * join_path_cmd - Build "dir/cmd" string.
 * @dir: Directory path.
 * @cmd: Command name.
 *
 * Return: Allocated full path, or NULL.
 */
static char *join_path_cmd(char *dir, char *cmd)
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
 * find_command - Resolve command through PATH.
 * @cmd: Command name or absolute/relative path.
 * @envp: Environment variables.
 *
 * Return: Allocated command path or NULL if not found.
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
		full = join_path_cmd(dir, cmd);
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
