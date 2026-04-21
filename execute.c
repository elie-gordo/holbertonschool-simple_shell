#include "shell.h"

/**
 * print_not_found - Print command not found error to stderr
 * @sh: shell state
 * @cmd: command that was not found
 */
void print_not_found(shell_t *sh, char *cmd)
{
	fprintf(stderr, "%s: %d: %s: not found\n",
		sh->name, sh->count, cmd);
}

/**
 * _child_exec - Execute command in child process
 * @args: argument vector
 * @cmd_path: resolved path to executable
 * @sh: shell state
 */
static void _child_exec(char **args, char *cmd_path, shell_t *sh)
{
	signal(SIGINT, SIG_DFL);
	execve(cmd_path, args, sh->env);
	perror(sh->name);
	free(cmd_path);
	_exit(127);
}

/**
 * execute_cmd - Fork and execute an external command
 * @args: argument vector (args[0] = command)
 * @sh: shell state
 *
 * Return: exit status of the command
 */
int execute_cmd(char **args, shell_t *sh)
{
	pid_t pid;
	int wstatus;
	char *cmd_path;

	cmd_path = find_command(args[0], sh);
	if (cmd_path == NULL)
	{
		print_not_found(sh, args[0]);
		return (127);
	}
	pid = fork();
	if (pid == -1)
	{
		free(cmd_path);
		perror(sh->name);
		return (1);
	}
	if (pid == 0)
		_child_exec(args, cmd_path, sh);
	waitpid(pid, &wstatus, 0);
	free(cmd_path);
	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
		return (128 + WTERMSIG(wstatus));
	return (1);
}
