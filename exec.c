/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 23:49:40 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 03:12:23 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_heredocs(t_cmd *cmd, t_env **env)
{
	t_redir	*r;
	int		fd;

	(void)env;
	r = cmd->redirs;
	while (r)
	{
		if (r->type == TOKEN_HEREDOC)
		{
			fd = heredoc_fd(r->file);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		r = r->next;
	}
}

void	exec_cmd(t_cmd *cmd, t_env **env)
{
	char	*path;
	char	**envp;

	setup_signals_child();
	handle_heredocs(cmd, env);
	if (apply_redirs(cmd->redirs) == -1)
		exit(1);
	if (is_builtin(cmd->args[0]))
		exit(run_builtin(cmd, env));
	path = find_path(cmd->args[0], *env);
	if (!path)
	{
		write(2, cmd->args[0], ft_strlen(cmd->args[0]));
		write(2, ": command not found\n", 20);
		exit(127);
	}
	envp = build_env(*env);
	execve(path, cmd->args, envp);
	exit(1);
}

int	exec_single(t_cmd *cmd, t_env **env)
{
	pid_t	pid;
	int		status;

	if (is_builtin(cmd->args[0]))
		return (builtin_with_redir(cmd, env));
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
		exec_cmd(cmd, env);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}

int	**alloc_pipes(int n)
{
	int	**pipes;
	int	i;

	pipes = malloc(sizeof(int *) * (n - 1));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < n - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		pipe(pipes[i]);
		i++;
	}
	return (pipes);
}

void	close_pipes(int **pipes, int n)
{
	int	i;

	i = 0;
	while (i < n - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}
