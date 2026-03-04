/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 23:08:35 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 03:12:23 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_redir(t_redir *r)
{
	if (r->type == TOKEN_REDIR_IN)
		return (open(r->file, O_RDONLY));
	if (r->type == TOKEN_REDIR_OUT)
		return (open(r->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	if (r->type == TOKEN_APPEND)
		return (open(r->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

int	apply_redirs(t_redir *redirs)
{
	int	fd;

	while (redirs)
	{
		if (redirs->type == TOKEN_HEREDOC)
		{
			redirs = redirs->next;
			continue ;
		}
		fd = open_redir(redirs);
		if (fd == -1)
			return (write(2, "minishell: open error\n", 22), -1);
		if (redirs->type == TOKEN_REDIR_IN)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		redirs = redirs->next;
	}
	return (0);
}

int	builtin_with_redir(t_cmd *cmd, t_env **env)
{
	int	sv[2];
	int	ret;

	sv[0] = dup(STDIN_FILENO);
	sv[1] = dup(STDOUT_FILENO);
	handle_heredocs(cmd, env);
	ret = 1;
	if (apply_redirs(cmd->redirs) != -1)
		ret = run_builtin(cmd, env);
	dup2(sv[0], STDIN_FILENO);
	dup2(sv[1], STDOUT_FILENO);
	close(sv[0]);
	close(sv[1]);
	return (ret);
}
