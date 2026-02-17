/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aganganu <aganganu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 23:30:56 by aganganu          #+#    #+#             */
/*   Updated: 2025/04/30 23:30:56 by aganganu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*start;
	t_list	*new_node;
	void	*content;

	if (!lst || !f || !del)
		return (NULL);
	start = NULL;
	while (lst)
	{
		content = f(lst->content);
		if (!content)
			return (ft_lstclear(&start, del), NULL);
		new_node = ft_lstnew(content);
		if (!new_node)
		{
			del(content);
			ft_lstclear(&start, del);
			return (NULL);
		}
		ft_lstadd_back(&start, new_node);
		lst = lst->next;
	}
	return (start);
}
