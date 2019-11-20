/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line_utils.c                              :+:    :+:            */
/*                                                     +:+                    */
/*   By: dpattij <dpattij@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/01 20:14:23 by dpattij        #+#    #+#                */
/*   Updated: 2019/11/03 18:59:52 by dpattij       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

t_vecstr		*vecstr_new(enum e_vecstr_tag tag)
{
	t_vecstr	*new_vecstr;

	new_vecstr = malloc(sizeof(t_vecstr));
	if (new_vecstr == NULL)
		return (NULL);
	new_vecstr->capacity = VECSTR_DEFAULT_CAPACITY;
	new_vecstr->length = 0;
	new_vecstr->tag = tag;
	if (tag == TaggedString)
		new_vecstr->raw.c = malloc(VECSTR_DEFAULT_CAPACITY * sizeof(char));
	else if (tag == TaggedVector)
		new_vecstr->raw.v = malloc(VECSTR_DEFAULT_CAPACITY * sizeof(void *));
	if (new_vecstr->raw.v == NULL)
		return (NULL);
	return (new_vecstr);
}

int				vecstr_resize(t_vecstr *self, t_size new_size)
{
	void		*new_raw;
	t_size		idx;

	new_raw = malloc(new_size *
		(self->tag == TaggedString ? sizeof(char) : sizeof(void *)));
	if (new_raw == NULL)
		return (STATUS_ERROR);
	idx = 0;
	while (idx < self->length)
	{
		if (self->tag == TaggedString)
			CAST(new_raw, char *)[idx] = self->raw.c[idx];
		else if (self->tag == TaggedVector)
			CAST(new_raw, void **)[idx] = self->raw.v[idx];
		idx += 1;
	}
	free(self->raw.v);
	self->raw.v = new_raw;
	self->capacity = new_size;
	return (STATUS_SUCCESS);
}

int				vecstr_push(t_vecstr *self, void *value)
{
	if (self->length >= self->capacity)
		if (vecstr_resize(self, self->capacity * 2) != 0)
			return (STATUS_ERROR);
	if (self->tag == TaggedString)
		self->raw.c[self->length] = (char)value;
	else if (self->tag == TaggedVector)
		self->raw.v[self->length] = value;
	self->length += 1;
	return (STATUS_SUCCESS);
}

int				vecstr_indirect_new(t_vecstr **buffer)
{
	if (*buffer == NULL)
		*buffer = vecstr_new(TaggedVector);
	if (*buffer == NULL)
		return (STATUS_ERROR);
	return (STATUS_SUCCESS);
}
