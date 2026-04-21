#include "shell.h"

volatile sig_atomic_t g_signal = 0;

/**
 * sigint_handler - Handle SIGINT (Ctrl+C)
 * @sig: signal number (unused)
 */
void sigint_handler(int sig)
{
	(void)sig;
	g_signal = 1;
	write(STDOUT_FILENO, "\n", 1);
}

/**
 * init_shell - Initialize shell state structure
 * @sh: pointer to shell state
 * @name: program name (argv[0])
 */
static void init_shell(shell_t *sh, char *name)
{
	memset(sh, 0, sizeof(shell_t));
	sh->name = name;
	sh->interactive = isatty(STDIN_FILENO);
	copy_environ(sh);
	load_history(sh);
}

/**
 * cleanup_shell - Free all shell resources
 * @sh: pointer to shell state
 */
static void cleanup_shell(shell_t *sh)
{
	int i;

	save_history(sh);
	free_history(sh);
	free_environ(sh);
	for (i = 0; i < sh->alias_count; i++)
		free(sh->aliases[i]);
}

/**
 * main - Entry point for the simple shell
 * @ac: argument count
 * @av: argument vector
 *
 * Return: last exit status
 */
int main(int ac, char **av)
{
	shell_t sh;

	init_shell(&sh, av[0]);
	signal(SIGINT, sigint_handler);
	if (ac >= 2)
		sh.status = file_mode(&sh, av[1]);
	else
		sh.status = shell_loop(&sh);
	cleanup_shell(&sh);
	return (sh.status);
}
