#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

extern char **environ;

/**
 * find_command - searches for a command in PATH or validates an absolute path
 * @command: the command name or path to find
 *
 * Return: heap-allocated full path string, or NULL if not found
 */
char *find_command(char *command)
{
	char *path_env, *path_copy, *dir, *full_path;
	int len;

	/* Handle absolute or relative paths directly */
	if (command[0] == '/' || command[0] == '.')
	{
		if (access(command, X_OK) == 0)
			return strdup(command);
		return NULL;
	}

	path_env = getenv("PATH");
	if (!path_env)
		return NULL;

	path_copy = strdup(path_env);
	if (!path_copy)
		return NULL;

	dir = strtok(path_copy, ":");
	while (dir)
	{
		len = strlen(dir) + strlen(command) + 2;
		full_path = malloc(len);
		if (!full_path)
		{
			free(path_copy); /* fix: was leaking path_copy on malloc failure */
			return NULL;
		}
		snprintf(full_path, len, "%s/%s", dir, command);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return full_path;
		}
		free(full_path);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return NULL;
}

/**
 * handle_builtin - checks for and executes built-in shell commands
 * @args: NULL-terminated array of argument strings
 *
 * Return: 1 if a built-in was handled, 0 otherwise
 */
int handle_builtin(char **args)
{
	int i;

	if (strcmp(args[0], "exit") == 0)
	{
		int code = 0;

		if (args[1])
			code = atoi(args[1]);
		exit(code);
	}

	if (strcmp(args[0], "env") == 0)
	{
		for (i = 0; environ[i]; i++)
			printf("%s\n", environ[i]);
		return 1;
	}

	return 0;
}

/**
 * main - entry point for the simple shell
 *
 * Return: 0 on success
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	char *args[MAX_ARGS];
	char *token, *cmd_path;
	pid_t pid;
	int status, i, interactive;

	interactive = isatty(STDIN_FILENO);

	while (1)
	{
		if (interactive)
		{
			printf(":) ");
			fflush(stdout);
		}

		if (getline(&line, &len, stdin) == -1)
			break;

		/* Strip trailing newline */
		line[strcspn(line, "\n")] = 0;

		/* Skip empty lines */
		if (strlen(line) == 0)
			continue;

		/* Tokenise the input line */
		i = 0;
		token = strtok(line, " \t");
		while (token && i < MAX_ARGS - 1)
		{
			args[i++] = token;
			token = strtok(NULL, " \t");
		}
		args[i] = NULL;

		/* Handle built-in commands before forking */
		if (handle_builtin(args))
			continue;

		/* Resolve the command path — do NOT fork if not found */
		cmd_path = find_command(args[0]);
		if (!cmd_path)
		{
			fprintf(stderr, "./shell: %s: command not found\n", args[0]);
			continue;
		}

		/* Fork only after confirming the command exists */
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			free(cmd_path);
			continue;
		}

		if (pid == 0)
		{
			/* Child: execute the command */
			execve(cmd_path, args, environ);
			perror("execve");
			free(cmd_path);
			exit(1);
		}
		else
		{
			/* Parent: wait for child to finish */
			waitpid(pid, &status, 0);
		}

		free(cmd_path);
	}

	free(line);
	return 0;
}
