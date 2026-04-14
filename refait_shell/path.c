#include "shell.h"

/**
 * get_env_value - Get an environment variable value.
 * @name: Variable name without '='.
 *
 * Return: Pointer to value in environ, or NULL.
 */
char *get_env_value(const char *name)
{
	int i;
	size_t key_len;

	if (name == NULL || environ == NULL)
		return (NULL);
	key_len = strlen(name);
	i = 0;
	while (environ[i])
	{
		if (strncmp(environ[i], name, key_len) == 0 && environ[i][key_len] == '=')
			return (environ[i] + key_len + 1);
		i++;
	}
	return (NULL);
}

/**
 * build_candidate - Build "directory/command" path.
 * @directory: Current directory from PATH.
 * @command: Command name.
 *
 * Return: Allocated path candidate or NULL.
 */
static char *build_candidate(char *directory, char *command)
{
	char *full_path;
	int size;

	size = strlen(directory) + strlen(command) + 2;
	full_path = malloc(size);
	if (full_path == NULL)
		return (NULL);
	sprintf(full_path, "%s/%s", directory, command);
	return (full_path);
}

/**
 * search_in_path - Search command in PATH entries.
 * @path_copy: Writable PATH copy.
 * @command: Command to search.
 *
 * Return: Allocated executable path or NULL.
 */
static char *search_in_path(char *path_copy, char *command)
{
	char *dir;
	char *candidate;

	dir = strtok(path_copy, ":");
	while (dir != NULL)
	{
		candidate = build_candidate(dir, command);
		if (candidate == NULL)
			return (NULL);
		if (access(candidate, X_OK) == 0)
			return (candidate);
		free(candidate);
		dir = strtok(NULL, ":");
	}
	return (NULL);
}

/**
 * resolve_command_path - Resolve an executable from command input.
 * @command: Input command.
 *
 * Return: Allocated executable path, or NULL.
 */
char *resolve_command_path(char *command)
{
	char *path_value;
	char *path_copy;
	char *result;

	if (command == NULL)
		return (NULL);
	if (strchr(command, '/') != NULL)
		return (access(command, X_OK) == 0 ? strdup(command) : NULL);
	path_value = get_env_value("PATH");
	if (path_value == NULL)
		return (NULL);
	path_copy = strdup(path_value);
	if (path_copy == NULL)
		return (NULL);
	result = search_in_path(path_copy, command);
	free(path_copy);
	return (result);
}
