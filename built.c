#include "shell.h"

/**
 * builtin_env - print all environment variables
 */
void builtin_env(void)
{
	int i;

	i = 0;
	while (environ[i] != NULL)
	{
		write(STDOUT_FILENO, environ[i], strlen(environ[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}
