#include "shell.h"

/**
 * print_not_found - Print "command not found" error to stderr
 * @sh: shell state (for program name and line count)
 * @cmd: the command that was not found
 *
 * Description: Format matches /bin/sh: "prog: line: cmd: not found"
 */
void print_not_found(shell_t *sh, char *cmd)
{
	fprintf(stderr, "%s: %d: %s: not found\n",
		sh->name, sh->count, cmd);
}

/**
 * _child_exec - Execute a command inside the child process
 * @args: argument vector for execve
 * @cmd_path: resolved full path to the executable
 * @sh: shell state (for environment and error messages)
 *
 * Description: Resets SIGINT to default (so child can be killed
 * by Ctrl+C), then calls execve. If execve fails, prints error
 * and exits the child with status 127.
 */
static void _child_exec(char **args, char *cmd_path, shell_t *sh)
{
	signal(SIGINT, SIG_DFL); /* Restore default Ctrl+C in child */
	execve(cmd_path, args, sh->env); /* Replace process image */
	perror(sh->name); /* execve failed: print system error */
	free(cmd_path); /* Clean up before exiting child */
	_exit(127); /* Exit child immediately (not exit()) */
}

/**
 * execute_cmd - Fork a child process and execute an external command
 * @args: argument vector (args[0] is the command name)
 * @sh: shell state
 *
 * Return: exit status of the command (127 if not found, 1 on fork error)
 *
 * Description: First resolves the command via PATH (no fork if not
 * found). Then forks: child calls execve, parent waits with waitpid.
 */
int execute_cmd(char **args, shell_t *sh)
{
	pid_t pid;
	int wstatus;
	char *cmd_path;

	cmd_path = find_command(args[0], sh); /* Resolve before fork */
	if (cmd_path == NULL) /* Command not found: don't fork */
	{
		print_not_found(sh, args[0]);
		return (127); /* Standard "not found" exit code */
	}
	pid = fork(); /* Create child process */
	if (pid == -1) /* Fork failed */
	{
		free(cmd_path);
		perror(sh->name);
		return (1);
	}
	if (pid == 0) /* Child process */
		_child_exec(args, cmd_path, sh);
	/* Parent process: wait for child to finish */
	waitpid(pid, &wstatus, 0);
	free(cmd_path); /* Free the resolved path */
	if (WIFEXITED(wstatus)) /* Child exited normally */
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus)) /* Child killed by signal */
		return (128 + WTERMSIG(wstatus));
	return (1); /* Fallback error code */
}
