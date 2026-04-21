#include "shell.h"

/**
 * _get_hist_path - Build path to history file (~/.simple_shell_history)
 * @sh: shell state
 *
 * Return: allocated path string, or NULL on failure
 */
static char *_get_hist_path(shell_t *sh)
{
	char *home, *path;
	int len;

	home = _getenv(sh, "HOME");
	if (home == NULL)
		return (NULL);
	len = strlen(home) + 24;
	path = malloc(len);
	if (path == NULL)
		return (NULL);
	sprintf(path, "%s/.simple_shell_history", home);
	return (path);
}

/**
 * load_history - Load history from file at startup
 * @sh: shell state
 *
 * Return: 0 on success, -1 on failure
 */
int load_history(shell_t *sh)
{
	char *path, *line = NULL;
	size_t len = 0;
	int fd;
	ssize_t nread;

	path = _get_hist_path(sh);
	if (path == NULL)
		return (-1);
	fd = open(path, O_RDONLY);
	free(path);
	if (fd == -1)
		return (-1);
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
			line[nread - 1] = '\0';
		if (sh->hist_count < HIST_MAX)
		{
			sh->hist[sh->hist_count] = strdup(line);
			sh->hist_count++;
		}
	}
	free(line);
	close(fd);
	return (0);
}

/**
 * save_history - Write history to file on exit
 * @sh: shell state
 *
 * Return: 0 on success, -1 on failure
 */
int save_history(shell_t *sh)
{
	char *path;
	int fd, i;

	if (sh->hist == NULL || sh->hist_count == 0)
		return (0);
	path = _get_hist_path(sh);
	if (path == NULL)
		return (-1);
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	free(path);
	if (fd == -1)
		return (-1);
	for (i = 0; i < sh->hist_count; i++)
	{
		write(fd, sh->hist[i], strlen(sh->hist[i]));
		write(fd, "\n", 1);
	}
	close(fd);
	return (0);
}

/**
 * add_history - Add a command line to history
 * @sh: shell state
 * @line: command line to add
 *
 * Return: 0 on success, -1 on failure
 */
int add_history(shell_t *sh, char *line)
{
	if (line == NULL || line[0] == '\0')
		return (0);
	if (sh->hist == NULL)
	{
		sh->hist = malloc(sizeof(char *) * (HIST_MAX + 1));
		if (sh->hist == NULL)
			return (-1);
		sh->hist_count = 0;
	}
	if (sh->hist_count >= HIST_MAX)
	{
		free(sh->hist[0]);
		memmove(sh->hist, sh->hist + 1,
			sizeof(char *) * (HIST_MAX - 1));
		sh->hist_count--;
	}
	sh->hist[sh->hist_count] = strdup(line);
	if (sh->hist[sh->hist_count] == NULL)
		return (-1);
	sh->hist_count++;
	return (0);
}

/**
 * free_history - Free all history entries
 * @sh: shell state
 */
void free_history(shell_t *sh)
{
	int i;

	if (sh->hist == NULL)
		return;
	for (i = 0; i < sh->hist_count; i++)
		free(sh->hist[i]);
	free(sh->hist);
	sh->hist = NULL;
	sh->hist_count = 0;
}
