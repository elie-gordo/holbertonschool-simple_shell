#include "shell.h"

/**
 * simple_shell - Main shell loop.
 * @prog_name: Program name used in error messages.
 * @envp: Environment variables.
 *
 * Return: Exit status.
 */
int simple_shell(char *prog_name, char **envp)
{
	char *line;
	size_t len;
	ssize_t nread;
	int interactive;
	int cmd_count;
	char **args;
	int should_exit;
	int exit_status;

	line = NULL;
	len = 0;
	interactive = isatty(STDIN_FILENO);
	cmd_count = 0;
	should_exit = 0;
	exit_status = 0;
	while (1)
	{
		if (interactive)
			write(STDOUT_FILENO, "($) ", 4);
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
		if (!handle_builtin(args, envp, &should_exit, &exit_status))
			exit_status = execute_command(args, prog_name, envp, cmd_count);
		free_args(args);
		if (should_exit)
			break;
	}
	if (interactive)
		write(STDOUT_FILENO, "\n", 1);
	free(line);
	return (exit_status);
}
