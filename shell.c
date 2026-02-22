#include "shell.h"

/**
 * parse_line - tokenize input line into argv array
 * @line: input line
 * @argv: array to fill with tokens
 *
 * Return: number of tokens
 */
int parse_line(char *line, char **argv)
{
	char *token;
	int i;

	i = 0;
	token = strtok(line, " \t");
	while (token != NULL && i < 99)
	{
		argv[i] = token;
		i++;
		token = strtok(NULL, " \t");
	}
	argv[i] = NULL;
	return (i);
}

/**
 * handle_builtins - checks and runs builtin commands
 * @argv: argument vector
 * @status: current exit status
 * @line: input line to free if needed
 *
 * Return: 1 if builtin ran, 0 otherwise
 */
int handle_builtins(char **argv, int *status, char *line)
{
	if (strcmp(argv[0], "exit") == 0)
	{
		free(line);
		exit(*status);
	}
	if (strcmp(argv[0], "env") == 0)
	{
		builtin_env();
		*status = 0;
		return (1);
	}
	return (0);
}

/**
 * run_command - finds and executes a command
 * @argv: argument vector
 * @status: pointer to exit status
 */
void run_command(char **argv, int *status)
{
	char *cmd_path;

	cmd_path = find_command(argv[0]);
	if (cmd_path == NULL)
	{
		fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
		*status = 127;
		return;
	}
	*status = exec_command(argv, cmd_path);
}

/**
 * main - simple shell
 *
 * Return: last command exit status
 */
int main(void)
{
	char *line;
	size_t len;
	ssize_t nread;
	int status;
	char *argv[100];

	line = NULL;
	len = 0;
	status = 0;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "newshell$ ", 10);

		nread = getline(&line, &len, stdin);
		if (nread == -1)
		{
			free(line);
			exit(status);
		}

		if (line[nread - 1] == '\n')
			line[nread - 1] = '\0';

		if (parse_line(line, argv) == 0)
			continue;

		if (handle_builtins(argv, &status, line))
			continue;

		run_command(argv, &status);
	}

	free(line);
	return (status);
}
