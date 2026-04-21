#include "shell.h"

/**
 * free_environ - Free the dynamic environment array and all entries
 * @sh: shell state
 *
 * Description: Frees each individual environment string,
 * then the array pointer itself. Resets count to 0.
 */
void free_environ(shell_t *sh)
{
	int i;

	if (sh->env == NULL)
		return; /* Nothing to free */
	for (i = 0; i < sh->env_count; i++)
		free(sh->env[i]); /* Free each "NAME=value" string */
	free(sh->env); /* Free the pointer array */
	sh->env = NULL;
	sh->env_count = 0;
}
