#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 64

extern char **environ;

/**
 * get_path - get PATH variable from environ
 * Return: PATH string or NULL
 */
char *get_path(void)
{
	int i = 0;

	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * find_path - find full path of command
 * @cmd: command
 * Return: full path or NULL
 */
char *find_path(char *cmd)
{
	char *path_env, *path_copy, *dir;
	static char full_path[1024];

	if (strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (cmd);
		return (NULL);
	}

	path_env = get_path();
	if (!path_env || *path_env == '\0')
		return (NULL);

	path_copy = strdup(path_env);
	if (!path_copy)
		return (NULL);

	dir = strtok(path_copy, ":");
	while (dir)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
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
 * print_env - prints environment variables
 */
void print_env(void)
{
	int i = 0;

	while (environ[i])
	{
		write(STDOUT_FILENO, environ[i], strlen(environ[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}

/**
 * main - simple shell with exit and env built-ins
 * @argc: argument count
 * @argv: argument vector
 * Return: exit status
 */
int main(int argc, char **argv)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t pid;
	int status = 0;
	int line_count = 0;
	char *args[MAX_ARGS];
	char *token;
	char *cmd_path;
	int i;

	(void)argc;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "$ ", 2);

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			free(line);
			exit(status);
		}

		line_count++;
		line[strcspn(line, "\n")] = '\0';

		i = 0;
		token = strtok(line, " ");
		while (token && i < MAX_ARGS - 1)
		{
			args[i++] = token;
			token = strtok(NULL, " ");
		}
		args[i] = NULL;

		if (args[0] == NULL)
			continue;

		/* EXIT BUILTIN */
		if (strcmp(args[0], "exit") == 0)
		{
			free(line);
			exit(status);
		}

		/* ENV BUILTIN */
		if (strcmp(args[0], "env") == 0)
		{
			print_env();
			continue;
		}

		cmd_path = find_path(args[0]);

		if (cmd_path == NULL)
		{
			fprintf(stderr, "%s: %d: %s: not found\n",
				argv[0], line_count, args[0]);
			status = 127;

			if (!isatty(STDIN_FILENO))
			{
				free(line);
				exit(127);
			}
			continue;
		}

		pid = fork();
		if (pid == 0)
		{
			execve(cmd_path, args, environ);
			perror(argv[0]);
			exit(127);
		}
		else
		{
			wait(&status);
			status = WEXITSTATUS(status);
		}
	}

	free(line);
	return (status);
}
