/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec3.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:22:00 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 01:25:35 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute(t_cmd *cmds, t_env **env)
{
	if (!cmds || !cmds->args || !cmds->args[0])
		return (0);
	if (cmds->next == NULL)
		return (exec_single(cmds, env));
	return (exec_pipeline(cmds, env));
}
