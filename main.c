#include "shell.h"

/**
 * main - Point d'entree du programme hsh.
 * @ac: Nombre d'arguments recus par le programme.
 * @av: Tableau des arguments (av[0] contient le nom du shell).
 * @envp: Tableau des variables d'environnement heritees.
 *
 * Return: Code de retour final du shell.
 *
 * Description:
 * Cette fonction delegue toute la logique du shell a simple_shell().
 * On ignore ac car le projet ne demande pas de flags en ligne de commande.
 */
int main(int ac, char **av, char **envp)
{
	(void)ac;
	return (simple_shell(av[0], envp));
}
