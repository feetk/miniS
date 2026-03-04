/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 00:12:32 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 03:37:07 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pipe_child(t_cmd *cmd, t_pctx *ctx, t_env **env)
{
	if (ctx->i > 0)
		dup2(ctx->pipes[ctx->i - 1][0], STDIN_FILENO);
	if (ctx->i < ctx->n - 1)
		dup2(ctx->pipes[ctx->i][1], STDOUT_FILENO);
	close_pipes(ctx->pipes, ctx->n);
	exec_cmd(cmd, env);
	exit(1);
}

int	count_cmds(t_cmd *cmd)
{
	int	n;

	n = 0;
	while (cmd && ++n)
		cmd = cmd->next;
	return (n);
}

static void	fork_cmds(t_cmd *cmds, t_pctx *ctx, pid_t *pids, t_env **env)
{
	while (cmds)
	{
		pids[ctx->i] = fork();
		if (pids[ctx->i] == 0)
			pipe_child(cmds, ctx, env);
		cmds = cmds->next;
		ctx->i++;
	}
}

static void	cleanup_pipeline(t_pctx *ctx, pid_t *pids)
{
	int	i;

	i = 0;
	while (i < ctx->n - 1)
		free(ctx->pipes[i++]);
	free(ctx->pipes);
	free(pids);
}

int	exec_pipeline(t_cmd *cmds, t_env **env)
{
	t_pctx	ctx;
	pid_t	*pids;
	int		status;

	ctx.n = count_cmds(cmds);
	ctx.pipes = alloc_pipes(ctx.n);
	pids = malloc(sizeof(pid_t) * ctx.n);
	ctx.i = 0;
	status = 0;
	fork_cmds(cmds, &ctx, pids, env);
	close_pipes(ctx.pipes, ctx.n);
	ctx.i = 0;
	while (ctx.i < ctx.n)
		waitpid(pids[ctx.i++], &status, 0);
	cleanup_pipeline(&ctx, pids);
	if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}
