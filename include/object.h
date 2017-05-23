#ifndef OBJECT_H_
# define OBJECT_H_

# include "new.h"

/*
** Get the object substruct
*/
# define OBJ(x) ((t_object *)(x))

/*
** Get the name
*/
# define OBJ_NAME(x) (get_object_name(OBJ(x)))

/*
** Compare two objects type
*/
# define OBJ_SAME(x, y) (is_same_objects(OBJ(x), OBJ(y)))

/*
** Get the string representation of the object
*/
# define OBJ_STR(x) (object_to_string(OBJ(x)))

/*
** Return the object name, or "Null"
*/
char	*get_object_name(t_object *o);

/*
** Compare objects name.
** Return -1 if at least one of them is NULL
** Return 1 if they are the same type (with same name)
** Return 0 otherwise
*/
int	is_same_objects(t_object *a, t_object *b);

/*
** Get the string representation of the object
*/
char	*object_to_string(t_object *o);

#endif /* OBJECT_H_ */
