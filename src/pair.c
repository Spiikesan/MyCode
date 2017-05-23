#include "pair.h"

t_pair		*t_pair_new(t_pair_init var)
{
  t_pair	*p;

  if ((p = newObject(t_pair)) == NULL)
    return (NULL);
  p->first = var.first;
  p->second = var.second;
  return (p);
}
