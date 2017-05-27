#include <string.h>
#include "map.h"

void	map_del(void *ptr)
{
  t_map	*m;

  if (!ptr)
    return ;
  m = ptr;
  free(m->array);
}

t_map	*t_map_new(t_map_init var)
{
  t_map	*m;

  if (!var.compar)
    return (NULL);
  if ((m = newObject(t_map, &map_del)) == NULL)
    return (NULL);
  ++var.size;
  if ((m->array = malloc(sizeof(t_pair *) * var.size)) == NULL)
    return (NULL);
  memset(m->array, 0, sizeof(t_pair *) * var.size);
  m->alloc = var.size;
  m->size = 0;
  m->compar = var.compar;
  m->defVal = var.defVal;
  return (m);
}

static void	_rearange(t_map *m, size_t i, size_t s)
{
  size_t	j;

  for (j = s - 1; j && j >= i; --j) {
    m->array[j]->first = m->array[j - 1]->first;
    m->array[j]->second = m->array[j - 1]->second;
  }
}

int		map_add(t_map *m, void *key, void *value)
{
  size_t	i;
  t_pair	*emplaced;

  if (!m || !key)
    return (MAP_ARGS);
  if (m->size >= m->alloc - 1)
    if ((m->array = realloc(m->array,
			    (++m->alloc) * sizeof(t_pair *))) == NULL)
      return (MAP_FUNC);
  ++(m->size);
  m->array[(m->size) - 1] = new(t_pair);
  emplaced = NULL;
  for (i = 0; i < m->size; ++i)
    if (emplaced)
      {
	_rearange(m, i, m->size);
	break;
      }
    else if (i < m->size - 1 && m->array[i] &&
	     m->compar(key, m->array[i]->first) < 0)
      emplaced = m->array[i];
  emplaced = (emplaced ? emplaced : m->array[(m->size) - 1]);
  emplaced->first = key;
  emplaced->second = value;
  return (MAP_NOERR);
}

int		map_remove(t_map *m, const void *key)
{
  size_t	i;
  int		found;

  if (!m || !key)
    return (MAP_ARGS);
  found = 0;
  for (i = 0; i < m->size; ++i)
    {
      if (!m->compar(key, m->array[i]->first))
	found = 1;
      if (found && i < m->size - 1)
	{
	  m->array[i]->first = m->array[i + 1]->first;
	  m->array[i]->second = m->array[i + 1]->second;
	}
    }
  if (!found)
    return (MAP_NOMORE);
  --(m->size);
  delete(m->array[m->size]);
  if (((m->alloc - 1) - m->size) >= MAP_MAX_DIFF && (m->alloc = (m->size + 1)))
    if ((m->array = realloc(m->array,
			    (m->alloc) * sizeof(t_pair *))) == NULL)
      return (MAP_FUNC);
  return (MAP_NOERR);
}
