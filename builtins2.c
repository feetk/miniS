/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins2.c                                         :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 02:30:50 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 02:30:50 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_env(t_env *env)
{
	while (env)
	{
		if (env->value)
		{
			write(1, env->key, ft_strlen(env->key));
			write(1, "=", 1);
			write(1, env->value, ft_strlen(env->value));
			write(1, "\n", 1);
		}
		env = env->next;
	}
	return (0);
}

static int	is_number(char *s)
{
	int	i;

	i = 0;
	if (s[i] == '-' || s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
		if (!ft_isdigit(s[i++]))
			return (0);
	return (1);
}

int	ft_exit(t_cmd *cmd)
{
	write(1, "exit\n", 5);
	if (!cmd->args[1])
		exit(g_last_exit);
	if (!is_number(cmd->args[1]))
	{
		write(2, "exit: numeric argument required\n", 32);
		exit(255);
	}
	if (cmd->args[2])
		return (write(2, "exit: too many arguments\n", 25), 1);
	exit(ft_atoi(cmd->args[1]) % 256);
}

void	update_env(t_env *env, char *key, char *val)
{
	while (env)
	{
		if (!ft_strncmp(env->key, key, ft_strlen(key) + 1))
		{
			free(env->value);
			env->value = ft_strdup(val);
			return ;
		}
		env = env->next;
	}
}

void	env_set(t_env **env, char *key, char *val)
{
	t_env	*node;

	node = *env;
	while (node)
	{
		if (!ft_strncmp(node->key, key, ft_strlen(key) + 1))
		{
			free(node->value);
			node->value = ft_strdup(val);
			return ;
		}
		node = node->next;
	}
	env_add_back(env, env_new(ft_strdup(key), ft_strdup(val)));
}
