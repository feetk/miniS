/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 15:52:36 by aganganu          #+#    #+#             */
/*   Updated: 2026/02/19 03:02:10 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return (s1[i] - s2[i]);
}

t_env	*env_new(char *key, char *value)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->key = key;
	new->value = value;
	new->next = NULL;
	return (new);
}

void	env_add_back(t_env **env, t_env *new)
{
	t_env	*temp;

	if (!new || !env)
		return ;
	if (!*env)
	{
		*env = new;
		return ;
	}
	temp = *env;
	while (temp->next)
		temp = temp->next;
	temp->next = new;
}

t_env	*init_minimal_env(void)
{
	t_env	*env_list;
	char	cwd[1024];

	env_list = NULL;
	if (getcwd(cwd, sizeof(cwd)))
		env_add_back(&env_list, env_new(ft_strdup("PWD"), ft_strdup(cwd)));
	env_add_back(&env_list, env_new(ft_strdup("SHLVL"), ft_strdup("1")));
	return (env_list);
}

t_env	*init_env(char **env_array)
{
	t_env	*env_list;
	int		i;
	int		j;
	char	*key;
	char	*value;

	i = 0;
	if (!env_array || !env_array[0])
		return (init_minimal_env());
	// if (!env_array || env_array[0])
	// 	return (init_minimal_env());
	env_list = NULL;
	while (env_array[i])
	{
		j = 0;
		while (env_array[i][j] && env_array[i][j] != '=')
			j++;
		key = ft_substr(env_array[i], 0, j);
		value = ft_strdup(env_array[i] + j + 1);
		env_add_back(&env_list, env_new(key, value));
		i++;
	}
	return (env_list);
}

char	*get_env_value(char *key, t_env *env)
{
	t_env	*temp;

	if (!key)
		return (NULL);
	temp = env;
	while (temp)
	{
		if (ft_strcmp(key, temp->key) == 0)
			return (temp->value);
		temp = temp->next;
	}
	return ("");
}
