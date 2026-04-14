#include "shell.h"

/**
 * print_not_found - Print command not found error.
 * @prog_name: Program name (argv[0]).
 * @line_no: Command index.
 * @cmd: Unknown command string.
 */
void print_not_found(char *prog_name, unsigned int line_no, char *cmd)
{
	fprintf(stderr, "%s: %u: %s: not found\n", prog_name, line_no, cmd);
}

/**
 * run_external - Fork and execute a resolved command path.
 * @path: Executable path.
 * @args: Parsed argument array.
 *
 * Return: Exit code.
 */
static int run_external(char *path, char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}
	if (pid == 0)
	{
		execve(path, args, environ);
		perror(args[0]);
		_exit(126);
	}
	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		return (1);
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}

/**
 * execute_line - Execute one parsed input line.
 * @args: Parsed command tokens.
 * @prog_name: Program name.
 * @line_no: Command index.
 * @exit_status: Last shell status.
 * @should_exit: Exit flag.
 *
 * Return: Updated status.
 */
int execute_line(char **args, char *prog_name, unsigned int line_no,
		int *exit_status, int *should_exit)
{
	char *path;
	int status;

	if (args == NULL || args[0] == NULL)
		return (*exit_status);
	if (handle_builtin(args, prog_name, line_no, exit_status, should_exit))
		return (*exit_status);
	path = resolve_command_path(args[0]);
	if (path == NULL)
	{
		print_not_found(prog_name, line_no, args[0]);
		return (127);
	}
	status = run_external(path, args);
	free(path);
	return (status);
}

/**
 * shell_loop - Run interactive/non-interactive shell loop.
 * @prog_name: Program name.
 *
 * Return: Shell exit status.
 */
int shell_loop(char *prog_name)
{
	char *line;
	size_t len;
	ssize_t nread;
	char **args;
	int interactive;
	int exit_status;
	int should_exit;
	unsigned int line_no;

	line = NULL;
	len = 0;
	interactive = isatty(STDIN_FILENO);
	exit_status = 0;
	should_exit = 0;
	line_no = 0;
	while (1)
	{
		if (interactive)
			write(STDOUT_FILENO, "($) ", 4);
		nread = getline(&line, &len, stdin);
		if (nread == -1)
			break;
		line_no++;
		args = parse_input(line);
		if (args == NULL)
			continue;
		exit_status = execute_line(args, prog_name, line_no, &exit_status,
				&should_exit);
		free_tokens(args);
		if (should_exit)
			break;
	}
	if (interactive)
		write(STDOUT_FILENO, "\n", 1);
	free(line);
	return (exit_status);
}
