#include "shell.h"

/**
 * simple_shell - Lance la boucle principale du shell.
 * @prog_name: Nom du programme (argv[0]) pour les erreurs.
 * @envp: Environnement a transmettre a execve.
 *
 * Return: Dernier code de statut connu.
 *
 * Description:
 * Cette fonction gere le mode interactif et non-interactif.
 * En mode interactif, elle affiche un prompt avant chaque lecture.
 * La ligne lue est parsee, puis executee (built-in ou binaire externe).
 */
int simple_shell(char *prog_name, char **envp)
{
	char *line;
	size_t len;
	ssize_t nread;
	int interactive;
	int cmd_count;
	char **args;
	int should_exit;
	int status;

	/* Initialisation des variables de controle. */
	line = NULL;
	len = 0;
	interactive = isatty(STDIN_FILENO);
	cmd_count = 0;
	should_exit = 0;
	status = 0;
	while (1)
	{
		/* Le prompt n'est affiche qu'en session interactive. */
		if (interactive)
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		/* Lecture d'une ligne complete (incluant eventuellement '\n'). */
		nread = getline(&line, &len, stdin);

		/* EOF ou erreur de lecture: sortie de la boucle. */
		if (nread == -1)
			break;

		/* Le numero de commande augmente a chaque ligne lue. */
		cmd_count++;

		/* Suppression du retour a la ligne final pour strtok. */
		if (nread > 0 && line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		/* Ligne vide: on repasse directement a l'iteration suivante. */
		if (line[0] == '\0')
			continue;

		/* Decoupage de la ligne en tokens. */
		args = parse_line(line);
		if (args == NULL)
			continue;

		/* Traitement prioritaire des built-ins. */
		if (!handle_builtin(args, envp, &should_exit))
			status = execute_command(args, prog_name, envp, cmd_count);

		/* Liberation du conteneur alloue par parse_line(). */
		free_args(args);

		/* Le built-in exit demande d'arreter la boucle. */
		if (should_exit)
			break;
	}

	/* Convention shell: nouvelle ligne visible apres Ctrl+D interactif. */
	if (interactive)
		write(STDOUT_FILENO, "\n", 1);

	/* Le buffer de getline doit etre libere en fin de programme. */
	free(line);
	return (status);
}
