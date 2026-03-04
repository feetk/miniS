/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsb <dsb@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 23:08:35 by dsb               #+#    #+#             */
/*   Updated: 2026/02/19 03:37:07 by dsb              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*env_to_str(t_env *node)
{
	char	*tmp;
	char	*val;
	char	*result;

	tmp = ft_strjoin(node->key, "=");
	val = node->value;
	if (!val)
		val = "";
	result = ft_strjoin(tmp, val);
	free(tmp);
	return (result);
}

char	**build_env(t_env *env)
{
	char	**tab;
	t_env	*tmp;
	int		i;

	i = 0;
	tmp = env;
	while (tmp && ++i)
		tmp = tmp->next;
	tab = malloc(sizeof(char *) * (i + 1));
	if (!tab)
		return (NULL);
	i = 0;
	while (env)
	{
		tab[i] = env_to_str(env);
		i++;
		env = env->next;
	}
	tab[i] = NULL;
	return (tab);
}

static char	*try_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	full = ft_strjoin(tmp, cmd);
	free(tmp);
	if (access(full, X_OK) == 0)
		return (full);
	free(full);
	return (NULL);
}

char	*find_path(char *cmd, t_env *env)
{
	char	**dirs;
	char	*path;
	int		i;

	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path = get_env_value("PATH", env);
	if (!path)
		return (NULL);
	dirs = ft_split(path, ':');
	i = 0;
	path = NULL;
	while (dirs[i] && !path)
		path = try_path(dirs[i++], cmd);
	i = 0;
	while (dirs[i])
		free(dirs[i++]);
	free(dirs);
	return (path);
}

void	free_env(t_env *env)
{
	t_env	*next;

	while (env)
	{
		next = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = next;
	}
}
