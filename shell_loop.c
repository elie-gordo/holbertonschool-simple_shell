#include "shell.h"

/**
 * _strip_newline - Remove trailing newline character from a string
 * @line: string to modify in place
 * @nread: number of characters in the string
 */
static void _strip_newline(char *line, ssize_t nread)
{
	if (nread > 0 && line[nread - 1] == '\n')
		line[nread - 1] = '\0'; /* Replace '\n' with null terminator */
}

/**
 * shell_loop - Main Read-Eval-Print Loop of the shell
 * @sh: pointer to shell state
 *
 * Return: last exit status
 *
 * Description: Displays prompt, reads input, processes commands.
 * Handles Ctrl+C (returns -2) and Ctrl+D (returns -1).
 */
int shell_loop(shell_t *sh)
{
	char *line = NULL;  /* Line buffer, allocated by _getline */
	size_t len = 0;     /* Buffer capacity */
	ssize_t nread;      /* Number of chars read */

	while (1)
	{
		if (sh->interactive)
			write(STDOUT_FILENO, "$ ", 2); /* Show prompt */
		nread = _getline(&line, &len, STDIN_FILENO); /* Read input */
		if (nread == -2)
			continue; /* Ctrl+C: signal interrupt, retry */
		if (nread == -1)
			break; /* Ctrl+D: EOF, exit loop */
		sh->count++; /* Increment line counter for errors */
		_strip_newline(line, nread); /* Remove trailing \n */
		if (line[0] == '\0')
			continue; /* Skip empty lines */
		add_history(sh, line); /* Save to history before processing */
		strip_comments(line); /* Remove # comments */
		if (line[0] == '\0')
			continue; /* Skip if only a comment */
		handle_operators(sh, line); /* Parse ; && || and execute */
		if (sh->should_exit)
			break; /* exit builtin was called */
	}
	if (sh->interactive)
		write(STDOUT_FILENO, "\n", 1); /* Clean newline on EOF */
	free(line); /* Free the _getline buffer */
	return (sh->status);
}

/**
 * file_mode - Execute commands read from a file
 * @sh: pointer to shell state
 * @filename: path to the file containing commands
 *
 * Return: last exit status
 *
 * Description: Opens the file, reads each line, and executes it.
 * No prompt is displayed in file mode.
 */
int file_mode(shell_t *sh, char *filename)
{
	int fd;             /* File descriptor for the command file */
	char *line = NULL;  /* Line buffer */
	size_t len = 0;     /* Buffer capacity */
	ssize_t nread;      /* Number of chars read */

	fd = open(filename, O_RDONLY); /* Open file read-only */
	if (fd == -1)
	{
		fprintf(stderr, "%s: 0: Can't open %s\n", sh->name, filename);
		return (127); /* File not found error code */
	}
	while (1)
	{
		nread = _getline(&line, &len, fd); /* Read next line */
		if (nread <= 0)
			break; /* EOF or error: stop reading */
		sh->count++; /* Increment line counter */
		_strip_newline(line, nread); /* Remove trailing \n */
		if (line[0] == '\0')
			continue; /* Skip empty lines */
		strip_comments(line); /* Remove # comments */
		if (line[0] == '\0')
			continue; /* Skip comment-only lines */
		handle_operators(sh, line); /* Parse and execute */
		if (sh->should_exit)
			break; /* exit was called */
	}
	free(line); /* Free the _getline buffer */
	close(fd); /* Close the command file */
	return (sh->status);
}
