/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 01:27:40 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 01:27:40 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char *cmd)
{
	return (!ft_strncmp(cmd, "echo", 5) || !ft_strncmp(cmd, "cd", 3)
		|| !ft_strncmp(cmd, "pwd", 4) || !ft_strncmp(cmd, "env", 4)
		|| !ft_strncmp(cmd, "exit", 5) || !ft_strncmp(cmd, "export", 7)
		|| !ft_strncmp(cmd, "unset", 6));
}

int	run_builtin(t_cmd *cmd, t_env **env)
{
	if (!ft_strncmp(cmd->args[0], "echo", 5))
		return (ft_echo(cmd));
	if (!ft_strncmp(cmd->args[0], "cd", 3))
		return (ft_cd(cmd, env));
	if (!ft_strncmp(cmd->args[0], "pwd", 4))
		return (ft_pwd());
	if (!ft_strncmp(cmd->args[0], "env", 4))
		return (ft_env(*env));
	if (!ft_strncmp(cmd->args[0], "exit", 5))
		return (ft_exit(cmd));
	if (!ft_strncmp(cmd->args[0], "export", 7))
		return (ft_export(cmd, env));
	return (ft_unset(cmd, env));
}

static int	is_n_flag(char *arg)
{
	int	i;

	if (!arg || arg[0] != '-' || !arg[1])
		return (0);
	i = 1;
	while (arg[i])
		if (arg[i++] != 'n')
			return (0);
	return (1);
}

int	ft_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	while (cmd->args[i] && is_n_flag(cmd->args[i]))
	{
		newline = 0;
		i++;
	}
	while (cmd->args[i])
	{
		write(1, cmd->args[i], ft_strlen(cmd->args[i]));
		if (cmd->args[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}

int	ft_pwd(void)
{
	char	cwd[4096];

	if (!getcwd(cwd, sizeof(cwd)))
		return (write(2, "pwd: error\n", 11), 1);
	write(1, cwd, ft_strlen(cwd));
	write(1, "\n", 1);
	return (0);
}
