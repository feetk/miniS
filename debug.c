/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:04:38 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/17 16:28:29 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_cmds(t_cmd *cmd)
{
	int		i;
	int		cmd_num = 1;
	t_redir	*redir;

	while (cmd)
	{
		printf("\n=== COMMAND %d ===\n", cmd_num++);
		i = 0;
		while (cmd->args && cmd->args[i])
		{
			printf("Arg[%d]:\t[%s]\n", i, cmd->args[i]);
			i++;
		}
		redir = cmd->redirs;
		while (redir)
		{
			printf("Redir:\tType %d, File: [%s]\n", redir->type, redir->file);
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	printf("==================\n\n");
}

void	free_cmds(t_cmd *cmd)
{
	t_cmd	*tmp;
	t_redir	*r_tmp;
	int		i;

	while (cmd)
	{
		tmp = cmd->next;
		if (cmd->args)
		{
			i = 0;
			while (cmd->args[i])
				free(cmd->args[i++]);
			free(cmd->args);
		}
		while (cmd->redirs)
		{
			r_tmp = cmd->redirs->next;
			free(cmd->redirs->file);
			free(cmd->redirs);
			cmd->redirs = r_tmp;
		}
		free(cmd);
		cmd = tmp;
	}
}
