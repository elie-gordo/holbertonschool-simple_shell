#include "shell.h"

/**
 * main - Entry point for the simple shell (hsh)
 * @ac: argument count (unused)
 * @av: argument vector, av[0] is the program name
 *
 * Return: last exit status from shell_loop
 *
 * Description: Simply delegates all logic to shell_loop,
 * passing the program name for error messages.
 */
int main(int ac, char **av)
{
	(void)ac; /* Not used: no command-line flags */
	return (shell_loop(av[0]));
}
