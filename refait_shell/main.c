#include "shell.h"

/**
 * main - Entry point for hsh.
 * @argc: Argument count.
 * @argv: Argument vector.
 *
 * Return: Shell exit status.
 */
int main(int argc, char **argv)
{
	(void)argc;
	return (shell_loop(argv[0]));
}
