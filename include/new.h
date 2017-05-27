#ifndef NEW_H_
# define NEW_H_

# include <stdlib.h>

/*
** Warning inutile, la norme de compilation exige que les champs
** non explicitement initialisés soient mis à 0. Norme que GCC respecte.
*/
# pragma GCC diagnostic ignored "-Wmissing-field-initializers"

typedef struct	s_object	t_object;

typedef void	(*del_func)(void *ptr);
typedef void	(*str_func)(t_object *o);

typedef struct	s_object_init
{
  size_t	obj_size;
  char		*name;
  del_func	del;
  str_func	str;
}		t_object_init;

struct		s_object
{
  t_object	*next;
  t_object	*prev;
  del_func	__delete__;
  str_func	__to_str__;
  char		*__name__;
};

t_object	*t_object_new(t_object_init var);
void		_delete(t_object *obj);
void		_reg_object(t_object *obj);
void		_unreg_object(t_object *obj);

# define new(type, ...) (type##_new((type##_init){__VA_ARGS__}))

# define newObject(type, ...) (type *)new(t_object, sizeof(type), #type , ##__VA_ARGS__)

# define delete(var) _delete((t_object *)var)

#endif /* !NEW_H_ */
