#include "shell.h"

/**
 * _strip_newline - Remove trailing newline from string
 * @line: string to modify
 * @nread: length of string
 */
static void _strip_newline(char *line, ssize_t nread)
{
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0';
}

/**
 * shell_loop - Main REPL loop for the shell
 * @sh: pointer to shell state
 *
 * Return: last exit status
 */
int shell_loop(shell_t *sh)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	while (1)
	{
		if (sh->interactive)
			write(STDOUT_FILENO, "$ ", 2);
		g_signal = 0;
		nread = _getline(&line, &len, STDIN_FILENO);
		if (nread == -1)
		{
			if (g_signal)
				continue;
			break;
		}
		sh->count++;
		_strip_newline(line, nread);
		if (line[0] == '\0')
			continue;
		add_history(sh, line);
		strip_comments(line);
		if (line[0] == '\0')
			continue;
		handle_operators(sh, line);
		if (sh->should_exit)
			break;
	}
	if (sh->interactive)
		write(STDOUT_FILENO, "\n", 1);
	free(line);
	return (sh->status);
}

/**
 * file_mode - Execute commands from a file
 * @sh: pointer to shell state
 * @filename: path to the command file
 *
 * Return: last exit status
 */
int file_mode(shell_t *sh, char *filename)
{
	int fd;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		fprintf(stderr, "%s: 0: Can't open %s\n", sh->name, filename);
		return (127);
	}
	while (1)
	{
		nread = _getline(&line, &len, fd);
		if (nread == -1)
			break;
		sh->count++;
		_strip_newline(line, nread);
		if (line[0] == '\0')
			continue;
		strip_comments(line);
		if (line[0] == '\0')
			continue;
		handle_operators(sh, line);
		if (sh->should_exit)
			break;
	}
	free(line);
	close(fd);
	return (sh->status);
}
