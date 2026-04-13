#include "shell.h"

/**
 * print_not_found - Prints standardized command-not-found error.
 * @prog_name: Program name used in error output.
 * @cmd_count: Current command number.
 * @cmd: Command name that failed resolution.
 */
void print_not_found(char *prog_name, int cmd_count, char *cmd)
{
	fprintf(stderr, "%s: %d: %s: not found\n", prog_name, cmd_count, cmd);
}

/**
 * execute_command - Forks and executes a parsed command line.
 * @args: Command argv array.
 * @prog_name: Program name used for perror output.
 * @envp: Environment variables passed to execve.
 * @cmd_count: Current command number.
 *
 * Return: Child status value.
 */
int execute_command(char **args, char *prog_name, char **envp, int cmd_count)
{
	pid_t pid;
	int status;
	char *cmd_path;

	cmd_path = find_command(args[0], envp);
	if (cmd_path == NULL)
	{
		print_not_found(prog_name, cmd_count, args[0]);
		return (127);
	}

	pid = fork();
	if (pid == -1)
	{
		free(cmd_path);
		perror(prog_name);
		return (1);
	}
	if (pid == 0)
	{
		execve(cmd_path, args, envp);
		perror(prog_name);
		_exit(127);
	}
	waitpid(pid, &status, 0);
	free(cmd_path);
	return (status);
}
