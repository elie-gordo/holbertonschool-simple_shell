#include "shell.h"

/**
 * execute_word - Executes one command word without arguments.
 * @word: Executable path entered by the user.
 * @prog_name: Program name used for perror output.
 * @envp: Environment variables passed to execve.
 *
 * Return: Child status value.
 */
int execute_word(char *word, char *prog_name, char **envp)
{
	pid_t pid;
	int status;
	char *argv[2];

	argv[0] = word;
	argv[1] = NULL;
	pid = fork();
	if (pid == -1)
	{
		perror(prog_name);
		return (1);
	}
	if (pid == 0)
	{
		execve(word, argv, envp);
		perror(prog_name);
		_exit(127);
	}
	waitpid(pid, &status, 0);
	return (status);
}
