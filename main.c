#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

extern char **environ;

/**
 * main - Simple UNIX shell
 * Return: Always 0
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	pid_t pid;
	int status;

	while (1)
	{
		if (isatty(STDIN_FILENO))
		{
			printf("#cisfun$ ");
			fflush(stdout);
		}

		nread = getline(&line, &len, stdin);

		/* Handle EOF (Ctrl+D) */
		if (nread == -1)
		{
			free(line);
			exit(0);
		}

		/* Array to hold the command and its arguments */
		char *argv[100];
		int i = 0;
		
		/* Tokenize the input string by spaces, tabs, and newlines */
		char *token = strtok(line, " \t\n");
		
		while (token != NULL && i < 99)
		{
			argv[i++] = token;
			token = strtok(NULL, " \t\n");
		}
		argv[i] = NULL; /* execve requires the array to be NULL-terminated */

		/* If no command was entered (e.g., user just hit Enter), prompt again */
		if (argv[0] == NULL)
			continue;

		pid = fork();

		if (pid == 0)
		{
			/* Child process */
			if (execve(argv[0], argv, environ) == -1)
			{
				perror("./hsh");
				free(line); /* Fixes the Valgrind memory leak before exiting */
				exit(1);
			}
		}
		else if (pid > 0)
		{
			/* Parent process */
			wait(&status);
		}
		else
		{
			perror("fork");
		}
	}

	return (0);
}
