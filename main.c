#include "shell.h"

/**
 * sigint_handler - Handle SIGINT signal (Ctrl+C)
 * @sig: signal number (unused)
 *
 * Description: Prints a newline so the prompt appears on a fresh line.
 * Uses write() because it is async-signal-safe (printf is not).
 */
void sigint_handler(int sig)
{
	(void)sig; /* Unused parameter */
	write(STDOUT_FILENO, "\n", 1); /* New line after ^C */
}

/**
 * init_shell - Initialize all fields of the shell state structure
 * @sh: pointer to shell state
 * @name: program name (argv[0])
 *
 * Description: Zeros out the struct, sets the program name,
 * checks if stdin is a terminal, copies the environment,
 * and loads command history from file.
 */
static void init_shell(shell_t *sh, char *name)
{
	memset(sh, 0, sizeof(shell_t)); /* Zero all fields */
	sh->name = name; /* Store program name for error messages */
	sh->interactive = isatty(STDIN_FILENO); /* Check if terminal */
	copy_environ(sh); /* Duplicate environ into sh->env */
	load_history(sh); /* Load ~/.simple_shell_history */
}

/**
 * cleanup_shell - Free all resources before exiting
 * @sh: pointer to shell state
 *
 * Description: Saves history to file, then frees history,
 * environment, and all alias strings.
 */
static void cleanup_shell(shell_t *sh)
{
	int i;

	save_history(sh); /* Write history to ~/.simple_shell_history */
	free_history(sh); /* Free history array */
	free_environ(sh); /* Free environment array */
	for (i = 0; i < sh->alias_count; i++)
		free(sh->aliases[i]); /* Free each alias string */
}

/**
 * main - Entry point for the simple shell
 * @ac: argument count
 * @av: argument vector
 *
 * Return: last exit status of executed command
 *
 * Description: Initializes the shell, sets up signal handling,
 * runs either file mode or interactive loop, cleans up, and exits.
 */
int main(int ac, char **av)
{
	shell_t sh;

	init_shell(&sh, av[0]); /* Prepare shell state */
	signal(SIGINT, sigint_handler); /* Catch Ctrl+C */
	if (ac >= 2)
		sh.status = file_mode(&sh, av[1]); /* Read from file */
	else
		sh.status = shell_loop(&sh); /* Interactive/pipe mode */
	cleanup_shell(&sh); /* Free all resources */
	return (sh.status); /* Return last command status */
}
