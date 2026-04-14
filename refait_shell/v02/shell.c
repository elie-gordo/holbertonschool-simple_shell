#include "shell.h"

/**
 * execute_command - Executes a command with arguments
 * @line: The command line to execute
 * @shell_name: Name of the shell program
 */
void execute_command(char *line, char *shell_name)
{
	pid_t pid;
	int status;
	char **args;

	args = parse_line(line);

	if (args[0] == NULL)
	{
		free_args(args);
		return;
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		free_args(args);
		exit(EXIT_FAILURE);
	}

	if (pid == 0)
	{
		execve(args[0], args, environ);
		fprintf(stderr, "%s: No such file or directory\n", shell_name);
		free_args(args);
		exit(EXIT_FAILURE);
	}
	else
	{
		wait(&status);
		free_args(args);
	}
}
