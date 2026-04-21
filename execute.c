#include "shell.h"

/**
 * print_not_found - Affiche l'erreur standard commande introuvable.
 * @prog_name: Nom du shell (argv[0]).
 * @cmd_count: Numero de la ligne de commande.
 * @cmd: Commande non resolue.
 */
void print_not_found(char *prog_name, int cmd_count, char *cmd)
{
	fprintf(stderr, "%s: %d: %s: not found\n", prog_name, cmd_count, cmd);
}

/**
 * execute_command - Execute une commande externe apres resolution PATH.
 * @args: Tableau argv (args[0] = commande).
 * @prog_name: Nom du shell pour les erreurs systeme.
 * @envp: Tableau d'environnement passe a execve.
 * @cmd_count: Numero de ligne pour les messages d'erreur.
 *
 * Return: Code de statut de la commande.
 */
int execute_command(char **args, char *prog_name, char **envp, int cmd_count)
{
	pid_t pid;
	int status;
	char *cmd_path;

	/* On evite fork si la commande n'existe pas (exigence task 0.3). */
	cmd_path = find_command(args[0], envp);
	if (cmd_path == NULL)
	{
		print_not_found(prog_name, cmd_count, args[0]);
		return (127);
	}

	pid = fork();
	if (pid == -1)
	{
		free(cmd_path);
		perror(prog_name);
		return (1);
	}

	if (pid == 0)
	{
		execve(cmd_path, args, envp);
		perror(prog_name);
		free(cmd_path);
		_exit(127);
	}

	waitpid(pid, &status, 0);
	free(cmd_path);

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
