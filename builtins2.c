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
	if (args[1] == NULL || args[2] == NULL)
	{
		fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
		sh->status = 1;
		return (1);
	}
	if (_setenv(sh, args[1], args[2]) == -1)
	{
		fprintf(stderr, "%s: setenv: failed\n", sh->name);
		sh->status = 1;
		return (1);
	}
	sh->status = 0;
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
	if (args[1] == NULL)
	{
		fprintf(stderr, "Usage: unsetenv VARIABLE\n");
		sh->status = 1;
		return (1);
	}
	_unsetenv(sh, args[1]);
	sh->status = 0;
	return (1);
}

/**
 * _do_cd - Perform the chdir and update PWD/OLDPWD
 * @sh: shell state
 * @dir: directory to change to
 *
 * Return: 0 on success, -1 on failure
 */
static int _do_cd(shell_t *sh, char *dir)
{
	char old[PATH_MAX], cur[PATH_MAX];

	if (getcwd(old, sizeof(old)) == NULL)
		old[0] = '\0';
	if (chdir(dir) == -1)
		return (-1);
	_setenv(sh, "OLDPWD", old);
	if (getcwd(cur, sizeof(cur)) != NULL)
		_setenv(sh, "PWD", cur);
	return (0);
}

/**
 * builtin_cd - Change current directory
 * @args: argument vector (cd [DIRECTORY])
 * @sh: shell state
 *
 * Return: 1 (handled)
 */
int builtin_cd(char **args, shell_t *sh)
{
	char *dir;

	if (args[1] == NULL)
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
	else if (strcmp(args[1], "-") == 0)
	{
		dir = _getenv(sh, "OLDPWD");
		if (dir == NULL)
		{
			fprintf(stderr, "%s: %d: cd: OLDPWD not set\n",
				sh->name, sh->count);
			sh->status = 2;
			return (1);
		}
		printf("%s\n", dir);
	}
	else
		dir = args[1];
	if (_do_cd(sh, dir) == -1)
	{
		fprintf(stderr, "%s: %d: cd: can't cd to %s\n",
			sh->name, sh->count, dir);
		sh->status = 2;
		return (1);
	}
	sh->status = 0;
	return (1);
}
