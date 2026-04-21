#include "shell.h"

/**
 * _build_path - Construct full path from directory and command
 * @dir: directory path
 * @cmd: command name
 *
 * Return: allocated string "dir/cmd", or NULL on failure
 */
static char *_build_path(char *dir, char *cmd)
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
 * _next_dir - Extract next directory from PATH string
 * @path: PATH string
 * @start: pointer to current position (updated after extraction)
 * @dir: buffer to write directory into
 *
 * Return: 1 if a directory was found, 0 if end of string
 */
static int _next_dir(char *path, int *start, char *dir)
{
	int i = *start, j = 0;

	if (path[i] == '\0')
		return (0);
	while (path[i] && path[i] != ':')
		dir[j++] = path[i++];
	dir[j] = '\0';
	if (path[i] == ':')
		i++;
	*start = i;
	return (1);
}

/**
 * _search_path - Search for command in PATH directories
 * @cmd: command name to find
 * @path_value: the PATH environment variable value
 *
 * Return: allocated full path if found, NULL otherwise
 */
static char *_search_path(char *cmd, char *path_value)
{
	int pos = 0;
	char dir[PATH_MAX], *full;

	while (_next_dir(path_value, &pos, dir))
	{
		if (dir[0] == '\0')
			strcpy(dir, ".");
		full = _build_path(dir, cmd);
		if (full && access(full, X_OK) == 0)
			return (full);
		free(full);
	}
	return (NULL);
}

/**
 * find_command - Resolve a command name to an executable path
 * @cmd: command name (may be absolute/relative or just a name)
 * @sh: shell state (for environment access)
 *
 * Return: allocated path string, or NULL if not found
 */
char *find_command(char *cmd, shell_t *sh)
{
	char *path_value;

	if (cmd == NULL || cmd[0] == '\0')
		return (NULL);
	if (strchr(cmd, '/') != NULL)
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}
	path_value = _getenv(sh, "PATH");
	if (path_value == NULL)
		return (NULL);
	return (_search_path(cmd, path_value));
}
