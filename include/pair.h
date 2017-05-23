#ifndef PAIR_H_
# define PAIR_H_

# include "new.h"

typedef struct	s_pair_init
{
  void		*first;
  void		*second;
}		t_pair_init;

typedef struct	s_pair
{
  t_object	__obj__;
  void		*first;
  void		*second;
}		t_pair;

t_pair		*t_pair_new(t_pair_init var);

#endif /* !PAIR_H_ */
