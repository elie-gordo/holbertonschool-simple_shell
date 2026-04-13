#include "shell.h"

/**
 * simple_shell - Runs shell loop in interactive/non-interactive mode.
 * @prog_name: Program name used for error messages.
 * @envp: Environment variables passed to execve.
 *
 * Return: 0 on normal EOF termination.
 */
int simple_shell(char *prog_name, char **envp)
{
	char *line;
	size_t len;
	ssize_t nread;
	int interactive;
	int cmd_count;
	char **args;

	line = NULL;
	len = 0;
	interactive = isatty(STDIN_FILENO);
	cmd_count = 0;
	while (1)
	{
		if (interactive)
			write(STDOUT_FILENO, "#cisfun$ ", 9);
		nread = getline(&line, &len, stdin);
		if (nread == -1)
			break;
		cmd_count++;
		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0';
		if (line[0] == '\0')
			continue;
		args = parse_line(line);
		if (args == NULL)
			continue;
		execute_command(args, prog_name, envp, cmd_count);
		free_args(args);
	}
	if (interactive)
		write(STDOUT_FILENO, "\n", 1);
	free(line);
	return (0);
}
