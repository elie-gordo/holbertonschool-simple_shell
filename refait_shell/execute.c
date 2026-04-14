#include "shell.h"

/**
 * print_not_found - Print not found error like /bin/sh format.
 * @prog_name: Program name.
 * @cmd_count: Command index.
 * @cmd: Missing command.
 */
void print_not_found(char *prog_name, int cmd_count, char *cmd)
{
	fprintf(stderr, "%s: %d: %s: not found\n", prog_name, cmd_count, cmd);
}

/**
 * execute_command - Execute external command.
 * @args: Parsed argv.
 * @prog_name: Program name for perror.
 * @envp: Environment variables.
 * @cmd_count: Command counter.
 *
 * Return: Exit status code.
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
		free(cmd_path);
		_exit(126);
	}
	waitpid(pid, &status, 0);
	free(cmd_path);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
