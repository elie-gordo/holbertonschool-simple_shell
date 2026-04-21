#include "shell.h"

/**
 * build_full_path - Construit "repertoire/commande".
 * @dir: Repertoire issu de PATH.
 * @cmd: Nom de commande a chercher.
 *
 * Return: Chaine allouee contenant le chemin complet, ou NULL.
 */
static char *build_full_path(char *dir, char *cmd)
{
	int len;
	char *full;

	len = strlen(dir) + strlen(cmd) + 2;
	full = malloc(len);
	if (full == NULL)
		return (NULL);

	sprintf(full, "%s/%s", dir, cmd);
	return (full);
}

/**
 * get_path_value - Recupere la valeur brute de PATH depuis envp.
 * @envp: Tableau d'environnement.
 *
 * Return: Pointeur vers la valeur de PATH, ou NULL si absente.
 */
static char *get_path_value(char **envp)
{
	int i;

	i = 0;
	while (envp[i] != NULL)
	{
		if (strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_command - Resout une commande en chemin executable.
 * @cmd: Commande demandee (nom simple ou chemin avec '/').
 * @envp: Tableau d'environnement.
 *
 * Return: Chemin alloue de la commande, ou NULL si introuvable.
 */
char *find_command(char *cmd, char **envp)
{
	char *path_value;
	char *paths;
	char *dir;
	char *full;

	/* Cas 1: l'utilisateur a fourni un chemin absolu/relatif. */
	if (strchr(cmd, '/') != NULL)
	{
		if (access(cmd, X_OK) == 0)
			return (strdup(cmd));
		return (NULL);
	}

	/* Cas 2: on cherche dans PATH. */
	path_value = get_path_value(envp);
	if (path_value == NULL)
		return (NULL);

	paths = strdup(path_value);
	if (paths == NULL)
		return (NULL);

	dir = strtok(paths, ":");
	while (dir != NULL)
	{
		full = build_full_path(dir, cmd);
		if (full != NULL && access(full, X_OK) == 0)
		{
			free(paths);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}

	free(paths);
	return (NULL);
}
