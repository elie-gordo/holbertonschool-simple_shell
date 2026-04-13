#include "shell.h"

/**
 * main - Entry point for the simple shell.
 * @ac: Number of arguments.
 * @av: Program arguments.
 * @envp: Environment variables.
 *
 * Return: Exit status code.
 */
int main(int ac, char **av, char **envp)
{
	(void)ac;
	return (simple_shell(av[0], envp));
}
