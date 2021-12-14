/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yshimazu <yshimazu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/29 16:26:31 by yshimazu          #+#    #+#             */
/*   Updated: 2021/12/14 17:17:41 by hyoshie          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static t_dict	*init_dict(void)
{
	t_dict	*elem;

	elem = xmalloc(sizeof(t_dict));
	elem->prev = elem;
	elem->next = elem;
	return (elem);
}

t_info	*shell_init(void)
{
	t_info	*info;
	extern char	**environ;
	
	info = xmalloc(sizeof(t_info));
	info->env = init_dict();
	info->exit_status = 0;
	add_envs(environ, info);

	return (info);
}
