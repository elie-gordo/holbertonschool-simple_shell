#include "shell.h"

/**
 * shell_loop - Main Read-Eval-Print Loop of the shell
 * @prog_name: program name (argv[0]) for error messages
 *
 * Return: last exit status
 *
 * Description: Displays a prompt in interactive mode, reads a line
 * with getline, parses it into tokens, and either handles a builtin
 * or executes an external command. Loops until EOF or exit.
 */
int shell_loop(char *prog_name)
{
	char *line = NULL;   /* Buffer for getline (auto-allocated) */
	size_t len = 0;      /* Buffer capacity managed by getline */
	ssize_t nread;       /* Number of chars read by getline */
	int interactive;     /* 1 if stdin is a terminal */
	int cmd_count = 0;   /* Line counter for error messages */
	int should_exit = 0; /* Flag set by exit builtin */
	int status = 0;      /* Last command exit status */
	char **args;         /* Parsed argument array */

	interactive = isatty(STDIN_FILENO); /* Check if terminal */
	while (1)
	{
		if (interactive)
			write(STDOUT_FILENO, "$ ", 2); /* Print prompt */
		nread = getline(&line, &len, stdin); /* Read one line */
		if (nread == -1)
			break; /* EOF (Ctrl+D) or error: exit loop */
		cmd_count++; /* Increment line number */
		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0'; /* Strip trailing newline */
		if (line[0] == '\0')
			continue; /* Skip empty lines */
		args = parse_line(line); /* Split into tokens */
		if (args == NULL)
			continue; /* Empty after parsing */
		if (!handle_builtin(args, &should_exit)) /* Try builtin */
			status = execute_cmd(args, prog_name, cmd_count);
		free_args(args); /* Free token array */
		if (should_exit)
			break; /* exit was called */
	}
	if (interactive)
		write(STDOUT_FILENO, "\n", 1); /* Clean newline on EOF */
	free(line); /* Free getline buffer */
	return (status);
}
