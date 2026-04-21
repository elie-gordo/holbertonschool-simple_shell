#include "shell.h"

/**
 * builtin_setenv - Set or modify an environment variable
 * @args: argument vector (setenv VARIABLE VALUE)
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
int builtin_setenv(char **args, shell_t *sh)
{
	if (args[1] == NULL || args[2] == NULL) /* Need both args */
	{
		fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
		sh->status = 1;
		return (1);
	}
	if (_setenv(sh, args[1], args[2]) == -1) /* Try to set */
	{
		fprintf(stderr, "%s: setenv: failed\n", sh->name);
		sh->status = 1;
		return (1);
	}
	sh->status = 0; /* Success */
	return (1);
}

/**
 * builtin_unsetenv - Remove an environment variable
 * @args: argument vector (unsetenv VARIABLE)
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
int builtin_unsetenv(char **args, shell_t *sh)
{
	if (args[1] == NULL) /* Need variable name */
	{
		fprintf(stderr, "Usage: unsetenv VARIABLE\n");
		sh->status = 1;
		return (1);
	}
	_unsetenv(sh, args[1]); /* Remove from our env array */
	sh->status = 0;
	return (1);
}

/**
 * _do_cd - Perform chdir and update PWD/OLDPWD environment variables
 * @sh: shell state
 * @dir: target directory path
 *
 * Return: 0 on success, -1 on failure
 */
static int _do_cd(shell_t *sh, char *dir)
{
	char old[PATH_MAX], cur[PATH_MAX];

	if (getcwd(old, sizeof(old)) == NULL)
		old[0] = '\0'; /* Save current dir as OLDPWD */
	if (chdir(dir) == -1)
		return (-1); /* chdir failed */
	_setenv(sh, "OLDPWD", old); /* Update OLDPWD to previous dir */
	if (getcwd(cur, sizeof(cur)) != NULL)
		_setenv(sh, "PWD", cur); /* Update PWD to new dir */
	return (0);
}

/**
 * builtin_cd - Change the current working directory
 * @args: argument vector (cd [DIRECTORY])
 * @sh: shell state
 *
 * Return: 1 (handled)
 *
 * Description: cd with no arg goes to $HOME. cd - goes to $OLDPWD
 * and prints the new directory. cd DIR changes to DIR.
 */
int builtin_cd(char **args, shell_t *sh)
{
	char *dir;

	if (args[1] == NULL) /* No argument: go to HOME */
	{
		dir = _getenv(sh, "HOME");
		if (dir == NULL)
		{
			fprintf(stderr, "%s: %d: cd: HOME not set\n",
				sh->name, sh->count);
			sh->status = 2;
			return (1);
		}
	}
	else if (strcmp(args[1], "-") == 0) /* cd - : go to OLDPWD */
	{
		dir = _getenv(sh, "OLDPWD");
		if (dir == NULL)
		{
			fprintf(stderr, "%s: %d: cd: OLDPWD not set\n",
				sh->name, sh->count);
			sh->status = 2;
			return (1);
		}
		printf("%s\n", dir); /* Print the directory we switch to */
	}
	else
		dir = args[1]; /* cd DIR: use the given directory */
	if (_do_cd(sh, dir) == -1) /* Perform the actual chdir */
	{
		fprintf(stderr, "%s: %d: cd: can't cd to %s\n",
			sh->name, sh->count, dir);
		sh->status = 2;
		return (1);
	}
	sh->status = 0; /* Success */
	return (1);
}
