#include "shell.h"

/**
 * exec_command - fork and execute a command
 * @argv: argument vector
 * @cmd_path: full path to the command
 *
 * Return: exit status of the child
 */
int exec_command(char **argv, char *cmd_path)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		if (execve(cmd_path, argv, environ) == -1)
		{
			perror("execve");
			exit(1);
		}
	}

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}
