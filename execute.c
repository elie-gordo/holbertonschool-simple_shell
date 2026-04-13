#include "shell.h"

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
	(void)cmd_count;

	pid = fork();
	if (pid == -1)
	{
		perror(prog_name);
		return (1);
	}
	if (pid == 0)
	{
		execve(args[0], args, envp);
		perror(prog_name);
		_exit(127);
	}
	waitpid(pid, &status, 0);
	return (status);
}
