#include "shell.h"

/**
 * build_full_path - Concatenate a directory and a command name
 * @dir: directory path from PATH (e.g. "/usr/bin")
 * @cmd: command name (e.g. "ls")
 *
 * Return: allocated string "dir/cmd", or NULL on failure
 */
static char *build_full_path(char *dir, char *cmd)
{
	int len;
	char *full;

	len = strlen(dir) + strlen(cmd) + 2; /* dir + / + cmd + \0 */
	full = malloc(len);
	if (full == NULL)
		return (NULL);
	sprintf(full, "%s/%s", dir, cmd); /* Join with / separator */
	return (full);
}

/**
 * get_path_value - Retrieve the value of PATH from environ
 *
 * Return: pointer to the PATH value string, or NULL if not set
 *
 * Description: Scans the global environ array for the entry
 * starting with "PATH=", then returns a pointer past the '='.
 */
static char *get_path_value(void)
{
	int i = 0;

	while (environ[i] != NULL)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5); /* Return value after '=' */
		i++;
	}
	return (NULL); /* PATH is not defined */
}

/**
 * search_in_path - Search for a command in all PATH directories
 * @cmd: command name to find
 * @path_copy: mutable copy of the PATH string (will be tokenized)
 *
 * Return: allocated full path if found, NULL otherwise
 */
static char *search_in_path(char *cmd, char *path_copy)
{
	char *dir;  /* Current directory from PATH */
	char *full; /* Constructed full path to test */

	dir = strtok(path_copy, ":"); /* Get first directory */
	while (dir != NULL)
	{
		full = build_full_path(dir, cmd); /* Build /dir/cmd */
		if (full != NULL && access(full, X_OK) == 0)
			return (full); /* Found executable: return it */
		free(full); /* Not here: try next directory */
		dir = strtok(NULL, ":"); /* Get next directory */
	}
	return (NULL); /* Command not found in any PATH dir */
}

/**
 * find_command - Resolve a command name to a full executable path
 * @cmd: command typed by the user
 *
 * Return: allocated path string, or NULL if not found
 *
 * Description: If cmd contains a '/', treat it as a direct path.
 * Otherwise, duplicate PATH and search each directory for cmd.
 * Does not fork if the command cannot be found (task 4 requirement).
 */
char *find_command(char *cmd)
{
	char *path_value; /* Raw PATH string from environ */
	char *path_copy;  /* Mutable copy for strtok */
	char *result;     /* Resolved path or NULL */

	if (strchr(cmd, '/') != NULL) /* Contains /: direct path */
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd)); /* Executable: return copy */
		return (NULL); /* Path given but not executable */
	}
	path_value = get_path_value(); /* Get PATH from environ */
	if (path_value == NULL)
		return (NULL); /* No PATH: cannot resolve */
	path_copy = strdup(path_value); /* Copy because strtok modifies */
	if (path_copy == NULL)
		return (NULL);
	result = search_in_path(cmd, path_copy); /* Search PATH dirs */
	free(path_copy); /* Free the PATH copy */
	return (result);
}
