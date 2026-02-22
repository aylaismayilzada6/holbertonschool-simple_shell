#include "shell.h"

/**
 * get_path_env - gets PATH value from environ
 *
 * Return: PATH string or NULL
 */
static char *get_path_env(void)
{
	int i;

	i = 0;
	while (environ[i] != NULL)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * search_in_dirs - searches command in PATH directories
 * @command: command name
 * @path_env: PATH string
 * @full_path: buffer to store result
 *
 * Return: full_path if found, NULL otherwise
 */
static char *search_in_dirs(char *command, char *path_env, char *full_path)
{
	char *path_copy;
	char *dir;

	path_copy = strdup(path_env);
	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir != NULL)
	{
		snprintf(full_path, 1024, "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}
		dir = strtok(NULL, ":");
	}
	free(path_copy);
	return (NULL);
}

/**
 * find_command - search command in PATH
 * @command: command name
 *
 * Return: full path if found, otherwise NULL
 */
char *find_command(char *command)
{
	char *path_env;
	static char full_path[1024];

	if (strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
			return (command);
		return (NULL);
	}

	path_env = get_path_env();
	if (path_env == NULL || path_env[0] == '\0')
		return (NULL);

	return (search_in_dirs(command, path_env, full_path));
}
