#include "vector.h"

size_t	vector_size(t_vector *v)
{
  if (!v)
    return (0);
  return (v->size);
}

size_t	vector_alloc(t_vector *v)
{
  if (!v)
    return (0);
  return (v->alloc);
}

void	*vector_at(t_vector *v, size_t index)
{
  if (!v || index >= v->size)
    return (NULL);
  return (v->array[index]);
}
