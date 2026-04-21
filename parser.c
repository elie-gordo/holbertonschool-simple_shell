#include "shell.h"

/**
 * count_tokens - Compte le nombre de mots separes par espace/tabulation.
 * @line: Ligne brute entree par l'utilisateur.
 *
 * Return: Nombre de tokens trouves, ou 0 en cas d'echec d'allocation.
 */
static int count_tokens(char *line)
{
	int count;
	char *copy;
	char *token;

	/* strtok modifie la chaine: on travaille donc sur une copie locale. */
	copy = strdup(line);
	if (copy == NULL)
		return (0);

	count = 0;
	token = strtok(copy, " \t");
	while (token != NULL)
	{
		count++;
		token = strtok(NULL, " \t");
	}

	free(copy);
	return (count);
}

/**
 * parse_line - Decoupe la ligne en tableau argv termine par NULL.
 * @line: Ligne a parser (sera modifiee par strtok).
 *
 * Return: Tableau de pointeurs vers tokens, ou NULL si ligne vide/echec.
 */
char **parse_line(char *line)
{
	int total;
	int i;
	char **args;
	char *token;

	total = count_tokens(line);
	if (total == 0)
		return (NULL);

	/* On alloue uniquement le conteneur; les mots pointent dans line. */
	args = malloc(sizeof(char *) * (total + 1));
	if (args == NULL)
		return (NULL);

	token = strtok(line, " \t");
	i = 0;
	while (token != NULL)
	{
		args[i] = token;
		i++;
		token = strtok(NULL, " \t");
	}
	args[i] = NULL;

	return (args);
}

/**
 * free_args - Libere le conteneur du tableau d'arguments.
 * @args: Tableau alloue par parse_line.
 */
void free_args(char **args)
{
	free(args);
}
