/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins3.c                                         :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 02:36:53 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 02:36:53 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	unset_one(t_env **env, char *key)
{
	t_env	*prev;
	t_env	*curr;

	prev = NULL;
	curr = *env;
	while (curr && ft_strncmp(curr->key, key, ft_strlen(key) + 1))
	{
		prev = curr;
		curr = curr->next;
	}
	if (!curr)
		return ;
	if (prev)
		prev->next = curr->next;
	else
		*env = curr->next;
	free(curr->key);
	free(curr->value);
	free(curr);
}

int	ft_unset(t_cmd *cmd, t_env **env)
{
	int	i;

	i = 1;
	while (cmd->args[i])
		unset_one(env, cmd->args[i++]);
	return (0);
}

void	print_export(t_env *env)
{
	while (env)
	{
		write(1, "declare -x ", 11);
		write(1, env->key, ft_strlen(env->key));
		if (env->value)
		{
			write(1, "=\"", 2);
			write(1, env->value, ft_strlen(env->value));
			write(1, "\"", 1);
		}
		write(1, "\n", 1);
		env = env->next;
	}
}

static t_env	*env_find(char *key, t_env *env)
{
	while (env)
	{
		if (!ft_strncmp(env->key, key, ft_strlen(key) + 1))
			return (env);
		env = env->next;
	}
	return (NULL);
}

int	export_one(char *arg, t_env **env)
{
	char	*eq;
	char	*key;
	char	*val;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (!env_find(arg, *env))
			env_add_back(env, env_new(ft_strdup(arg), NULL));
		return (0);
	}
	key = ft_substr(arg, 0, eq - arg);
	val = ft_strdup(eq + 1);
	if (env_find(key, *env))
	{
		update_env(*env, key, val);
		free(key);
		free(val);
	}
	else
		env_add_back(env, env_new(key, val));
	return (0);
}
