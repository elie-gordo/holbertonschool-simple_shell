#include "shell.h"

/**
 * _get_hist_path - Build the path to the history file
 * @sh: shell state (for $HOME lookup)
 *
 * Return: allocated path string, or NULL on failure
 */
static char *_get_hist_path(shell_t *sh)
{
	char *home, *path;
	int len;

	home = _getenv(sh, "HOME"); /* Get user home directory */
	if (home == NULL)
		return (NULL);
	len = strlen(home) + 24; /* home + "/.simple_shell_history" + \0 */
	path = malloc(len);
	if (path == NULL)
		return (NULL);
	sprintf(path, "%s/.simple_shell_history", home); /* Build path */
	return (path);
}

/**
 * load_history - Load command history from file at shell startup
 * @sh: shell state
 *
 * Return: 0 on success, -1 on failure
 *
 * Description: Reads ~/.simple_shell_history line by line using
 * _getline and stores each line in the hist array.
 */
int load_history(shell_t *sh)
{
	char *path, *line = NULL;
	size_t len = 0;
	int fd;
	ssize_t nread;

	path = _get_hist_path(sh); /* Build history file path */
	if (path == NULL)
		return (-1);
	fd = open(path, O_RDONLY); /* Open history file */
	free(path);
	if (fd == -1)
		return (-1); /* File doesn't exist yet: OK */
	sh->hist = malloc(sizeof(char *) * (HIST_MAX + 1));
	if (sh->hist == NULL)
	{
		close(fd);
		return (-1);
	}
	sh->hist_count = 0;
	while ((nread = _getline(&line, &len, fd)) != -1)
	{
		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0'; /* Strip trailing newline */
		if (sh->hist_count < HIST_MAX)
		{
			sh->hist[sh->hist_count] = strdup(line); /* Store */
			sh->hist_count++;
		}
	}
	free(line); /* Free _getline buffer */
	close(fd);  /* Close history file */
	return (0);
}

/**
 * save_history - Write command history to file on shell exit
 * @sh: shell state
 *
 * Return: 0 on success, -1 on failure
 */
int save_history(shell_t *sh)
{
	char *path;
	int fd, i;

	if (sh->hist == NULL || sh->hist_count == 0)
		return (0); /* Nothing to save */
	path = _get_hist_path(sh); /* Build history file path */
	if (path == NULL)
		return (-1);
	/* Create/truncate the history file with 0644 permissions */
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(path);
	if (fd == -1)
		return (-1);
	for (i = 0; i < sh->hist_count; i++) /* Write each entry */
	{
		write(fd, sh->hist[i], strlen(sh->hist[i]));
		write(fd, "\n", 1); /* One command per line */
	}
	close(fd);
	return (0);
}

/**
 * add_history - Add a command line to the in-memory history
 * @sh: shell state
 * @line: command line string to add
 *
 * Return: 0 on success, -1 on failure
 *
 * Description: If history is full (HIST_MAX), removes the oldest
 * entry by shifting the array and adds the new one at the end.
 */
int add_history(shell_t *sh, char *line)
{
	if (line == NULL || line[0] == '\0')
		return (0); /* Don't add empty lines */
	if (sh->hist == NULL) /* First call: allocate the array */
	{
		sh->hist = malloc(sizeof(char *) * (HIST_MAX + 1));
		if (sh->hist == NULL)
			return (-1);
		sh->hist_count = 0;
	}
	if (sh->hist_count >= HIST_MAX) /* History full: remove oldest */
	{
		free(sh->hist[0]);
		memmove(sh->hist, sh->hist + 1,
			sizeof(char *) * (HIST_MAX - 1)); /* Shift left */
		sh->hist_count--;
	}
	sh->hist[sh->hist_count] = strdup(line); /* Add at end */
	if (sh->hist[sh->hist_count] == NULL)
		return (-1);
	sh->hist_count++;
	return (0);
}

/**
 * free_history - Free all history entries and the array
 * @sh: shell state
 */
void free_history(shell_t *sh)
{
	int i;

	if (sh->hist == NULL)
		return;
	for (i = 0; i < sh->hist_count; i++)
		free(sh->hist[i]); /* Free each history string */
	free(sh->hist); /* Free the array itself */
	sh->hist = NULL;
	sh->hist_count = 0;
}
