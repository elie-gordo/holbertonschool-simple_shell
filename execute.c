#include "shell.h"

/**
 * print_not_found - Print "command not found" error to stderr
 * @prog_name: shell program name (argv[0])
 * @cmd_count: current line number
 * @cmd: the command that was not found
 *
 * Description: Error format matches /bin/sh output:
 * "./hsh: 1: xyz: not found"
 */
void print_not_found(char *prog_name, int cmd_count, char *cmd)
{
	fprintf(stderr, "%s: %d: %s: not found\n",
		prog_name, cmd_count, cmd);
}

/**
 * execute_cmd - Fork a child process and execute an external command
 * @args: NULL-terminated argument vector (args[0] = command)
 * @prog_name: shell name for error messages
 * @cmd_count: line number for error messages
 *
 * Return: exit status of the command (127 if not found)
 *
 * Description: Resolves the command via PATH before forking.
 * If the command is not found, prints an error and returns 127
 * without forking (task 4 requirement). Child calls execve
 * with environ; parent waits with waitpid.
 */
int execute_cmd(char **args, char *prog_name, int cmd_count)
{
	pid_t pid;      /* Process ID from fork */
	int wstatus;    /* Wait status from waitpid */
	char *cmd_path; /* Resolved full path to executable */

	cmd_path = find_command(args[0]); /* Resolve before fork */
	if (cmd_path == NULL) /* Not found: don't fork */
	{
		print_not_found(prog_name, cmd_count, args[0]);
		return (127); /* Standard "not found" exit code */
	}
	pid = fork(); /* Create child process */
	if (pid == -1) /* Fork failed */
	{
		free(cmd_path);
		perror(prog_name); /* Print system error */
		return (1);
	}
	if (pid == 0) /* Child process */
	{
		execve(cmd_path, args, environ); /* Replace with command */
		perror(prog_name); /* execve failed */
		free(cmd_path);
		_exit(127); /* Exit child (not exit() to avoid flush) */
	}
	/* Parent process */
	waitpid(pid, &wstatus, 0); /* Wait for child to finish */
	free(cmd_path); /* Free resolved path */
	if (WIFEXITED(wstatus)) /* Child exited normally */
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus)) /* Child killed by signal */
		return (128 + WTERMSIG(wstatus));
	return (1); /* Fallback */
}
