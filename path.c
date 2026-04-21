#include "shell.h"

/**
 * _build_path - Construct a full path by joining directory and command
 * @dir: directory path (e.g. "/usr/bin")
 * @cmd: command name (e.g. "ls")
 *
 * Return: allocated string "dir/cmd", or NULL on failure
 */
static char *_build_path(char *dir, char *cmd)
{
	int len;
	char *full;

	len = strlen(dir) + strlen(cmd) + 2; /* dir + / + cmd + \0 */
	full = malloc(len);
	if (full == NULL)
		return (NULL);
	sprintf(full, "%s/%s", dir, cmd); /* Concatenate with / */
	return (full);
}

/**
 * _next_dir - Extract the next directory from a PATH string
 * @path: the PATH value (colon-separated directories)
 * @start: pointer to current scan position (updated after call)
 * @dir: buffer to write the extracted directory into
 *
 * Return: 1 if a directory was extracted, 0 if end of PATH
 *
 * Description: Manually parses PATH without strtok by scanning
 * for the next colon delimiter or end of string.
 */
static int _next_dir(char *path, int *start, char *dir)
{
	int i = *start, j = 0;

	if (path[i] == '\0')
		return (0); /* End of PATH string */
	while (path[i] && path[i] != ':')
		dir[j++] = path[i++]; /* Copy chars until colon or end */
	dir[j] = '\0'; /* Null-terminate the directory */
	if (path[i] == ':')
		i++; /* Skip the colon delimiter */
	*start = i; /* Update position for next call */
	return (1);
}

/**
 * _search_path - Search for a command in all PATH directories
 * @cmd: command name to look for
 * @path_value: the PATH environment variable value
 *
 * Return: allocated full path if the command is found, NULL otherwise
 */
static char *_search_path(char *cmd, char *path_value)
{
	int pos = 0;
	char dir[PATH_MAX], *full;

	while (_next_dir(path_value, &pos, dir)) /* Try each directory */
	{
		if (dir[0] == '\0')
			strcpy(dir, "."); /* Empty entry means current dir */
		full = _build_path(dir, cmd); /* Build /dir/cmd */
		if (full && access(full, X_OK) == 0)
			return (full); /* Found an executable: return it */
		free(full); /* Not found here: try next directory */
	}
	return (NULL); /* Command not in any PATH directory */
}

/**
 * find_command - Resolve a command name to an executable path
 * @cmd: command (may be absolute/relative path or just a name)
 * @sh: shell state (for environment access)
 *
 * Return: allocated path string, or NULL if command not found
 *
 * Description: If the command contains a /, it is treated as a
 * direct path. Otherwise, it is searched in PATH directories.
 */
char *find_command(char *cmd, shell_t *sh)
{
	char *path_value;

	if (cmd == NULL || cmd[0] == '\0')
		return (NULL); /* No command given */
	if (strchr(cmd, '/') != NULL) /* Contains /: direct path */
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd)); /* Executable: return copy */
		return (NULL); /* Path given but not executable */
	}
	path_value = _getenv(sh, "PATH"); /* Get PATH from our env */
	if (path_value == NULL)
		return (NULL); /* No PATH defined */
	return (_search_path(cmd, path_value)); /* Search in PATH */
}
