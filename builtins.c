#include "shell.h"

/**
 * print_env - Affiche toutes les variables d'environnement.
 * @envp: Tableau envp recu par le programme.
 */
static void print_env(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
	{
		printf("%s\n", envp[i]);
		i++;
	}
}

/**
 * handle_builtin - Gere les built-ins supportes par le projet.
 * @args: Arguments parsees de la ligne courante.
 * @envp: Environnement courant.
 * @should_exit: Flag de sortie a activer pour "exit".
 *
 * Return: 1 si built-in traite, 0 sinon.
 *
 * Description:
 * Task 0.4: support de "exit" sans gestion d'arguments obligatoire.
 * Task 1.0: support de "env" pour afficher l'environnement.
 */
int handle_builtin(char **args, char **envp, int *should_exit)
{
	if (strcmp(args[0], "exit") == 0)
	{
		*should_exit = 1;
		return (1);
	}

	if (strcmp(args[0], "env") == 0)
	{
		print_env(envp);
		return (1);
	}

	return (0);
}
