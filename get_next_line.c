/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: dpattij <dpattij@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/01 20:34:49 by dpattij        #+#    #+#                */
/*   Updated: 2019/11/03 02:35:44 by dpattij       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"
#include <stdio.h>

static int			get_fd_buffer(t_vecstr *buffers, int fd, t_pairedstr **out)
{
	t_size			idx;
	t_pairedstr		*current;

	idx = 0;
	current = buffers->raw.v[buffers->length ? buffers->length - 1 : 0];
	while (buffers->length != 0 && idx < buffers->length && current->key != fd)
	{
		current = buffers->raw.v[buffers->length - idx - 1];
		idx += 1;
	}
	if (idx == buffers->length)
	{
		current = malloc(sizeof(t_pairedstr));
		if (current == NULL)
			return (STATUS_ERROR);
		current->key = fd;
		current->string = malloc(sizeof(char) * BUFFER_SIZE);
		current->head = 0;
		if (current->string == NULL || vecstr_push(buffers, current) != 0)
			return (STATUS_ERROR);
		*out = current;
		return (STATUS_BUFFER_IS_NEW);
	}
	*out = current;
	return (STATUS_BUFFER_IS_OLD);
}

static int			refit_buffer(t_vecstr *buffers, t_pairedstr *collected)
{
	t_size			idx;

	idx = 0;
	while (buffers->raw.v[idx] != collected)
		idx += 1;
	while (idx < (buffers->length) - 1)
	{
		buffers->raw.v[idx] = buffers->raw.v[idx + 1];
		idx -= 1;
	}
	free(collected);
	buffers->length -= 1;
	return (vecstr_resize(buffers, buffers->length));
}

static int			append_to_buffer
(
	t_pairedstr *cur_buf,
	t_vecstr *cur_line,
	t_size n
)
{
	t_size			idx;

	idx = cur_buf->head;
	while (idx < BUFFER_SIZE && idx < (cur_buf->head + n))
	{
		if (cur_buf->string[idx] == '\n')
		{
			cur_buf->head = idx + 1;
			return (STATUS_LINE_READ);
		}
		if (vecstr_push(cur_line, (void *)(t_size)(cur_buf->string[idx])) != 0)
			return (STATUS_ERROR);
		idx += 1;
	}
	cur_buf->head = 0;
	return (STATUS_SUCCESS);
}

static int			read_until_newline
(
	int fd,
	t_pairedstr *cur_buf,
	t_vecstr *cur_line,
	char **line
)
{
	t_ssize			result;

	result = fd == -1 ? -1 : read(fd, cur_buf->string, BUFFER_SIZE);
	while (result > 0)
	{
		result = append_to_buffer(cur_buf, cur_line, result);
		if (result == STATUS_SUCCESS)
			result = read(fd, cur_buf->string, BUFFER_SIZE);
		else
			break ;
	}
	if (result >= 0 || fd == -1)
	{
		vecstr_push(cur_line, NULL);
		vecstr_resize(cur_line, cur_line->length);
		*line = cur_line->raw.c;
		free(cur_line);
	}
	return (result);
}

int					get_next_line(int fd, char **line)
{
	static t_vecstr	*buffers;
	t_vecstr		*cur_line;
	t_pairedstr		*cur_buf;
	t_ssize			result;

	if (fd < 0 || vecstr_indirect_new(&buffers) != 0)
		return (STATUS_ERROR);
	cur_line = vecstr_new(TaggedString);
	result = get_fd_buffer(buffers, fd, &cur_buf);
	if (cur_line == NULL || result == STATUS_ERROR)
		return (STATUS_ERROR);
	if (result == STATUS_BUFFER_IS_OLD)
	{
		result = append_to_buffer(cur_buf, cur_line, BUFFER_SIZE);
		if (result == STATUS_LINE_READ)
		{
			read_until_newline(-1, cur_buf, cur_line, line);
			return (result);
		}
	}
	result = read_until_newline(fd, cur_buf, cur_line, line);
	if (result == STATUS_EOF_REACHED)
		refit_buffer(buffers, cur_buf);
	return (result);
}
