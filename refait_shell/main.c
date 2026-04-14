#include "shell.h"

/**
 * main - Entry point.
 * @ac: Argument count.
 * @av: Argument vector.
 * @envp: Environment variables.
 *
 * Return: Shell exit status.
 */
int main(int ac, char **av, char **envp)
{
	(void)ac;
	return (simple_shell(av[0], envp));
}
