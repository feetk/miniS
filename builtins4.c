/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins4.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 03:00:00 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 03:00:00 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_cd(t_cmd *cmd, t_env **env)
{
	char	*target;
	char	cwd[4096];

	target = cmd->args[1];
	if (!target)
		target = get_env_value("HOME", *env);
	else if (ft_strncmp(target, "-", 2) == 0)
		target = get_env_value("OLDPWD", *env);
	if (!target || !target[0])
		return (write(2, "cd: HOME not set\n", 17), 1);
	getcwd(cwd, sizeof(cwd));
	if (chdir(target) == -1)
		return (write(2, "cd: no such file or directory\n", 30), 1);
	env_set(env, "OLDPWD", cwd);
	getcwd(cwd, sizeof(cwd));
	env_set(env, "PWD", cwd);
	return (0);
}

int	ft_export(t_cmd *cmd, t_env **env)
{
	int	i;

	if (!cmd->args[1])
		return (print_export(*env), 0);
	i = 1;
	while (cmd->args[i])
		export_one(cmd->args[i++], env);
	return (0);
}
