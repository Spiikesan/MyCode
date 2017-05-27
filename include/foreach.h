#ifndef FOREACH_H_
# define FOREACH_H_

# include "new.h"

/*
** To correctly work with foreach, the container must be in the form :
** struct s_<container>
** {
**   t_object	__obj__;
**   size_t	nElemsInArray;
**   void	**arrayOfElements;
**   ...
** };
*/

/*
** Utilities for the FOREACH macro
*/
# define FIRST(c) (*(void ***)((void *)c + sizeof(t_object) + sizeof(size_t)))
# define LAST(c) (FIRST(c) + *((size_t *)(((void *)c) + sizeof(t_object))))

# define FE_INIT(t, e, c) void **_cur_ = FIRST(c); for (t e = (t)*_cur_
# define FE_COND(t, e, c) _cur_ != LAST(c)
# define FE_END(t, e) e = (t)*(++_cur_))

# define FOREACH(t, e, c) FE_INIT(t, e, c); FE_COND(t, e, c); FE_END(t, e)

#endif /* !FOREACH_H_ */
