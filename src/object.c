#include <string.h>
#include "object.h"

char	*get_object_name(t_object *o)
{
  if (!o)
    return ("Null");
  return (o->__name__);
}

int	is_same_objects(t_object *a, t_object *b)
{
  if (!a || !b)
    return (-1);
  return (strcmp(getObjectName(a), getObjectName(b)) == 0);
}

char	*object_to_string(t_object *o)
{
  if (!o || !o->__to_str__)
    return (getObjectName(o));
  return (o->__to_str__(o));
}
