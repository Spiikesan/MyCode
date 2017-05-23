#include "map.h"

size_t	map_size(const t_map *m)
{
  return (m->size);
}

void		*map_get(const t_map *m, const void *key)
{
  size_t	i;

  for (i = 0; i < m->size; ++i)
    {
      if (m->array[i] && !m->compar(key, m->array[i]->first))
	return (m->array[i]->second);
    }
  return (m->defVal);
}
