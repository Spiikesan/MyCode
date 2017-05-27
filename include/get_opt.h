#ifndef GET_OPT_H_
# define GET_OPT_H_

# include "opt_types.h"

# include "new.h"
# include "vector.h"

# define OPT_NOERR	(0)
# define OPT_PARAMS	(1)
# define OPT_FUNC	(2)
# define OPT_EXIST	(3)
# define OPT_NOTFOUND	(4)
# define OPT_NOTSET	(5)
# define OPT_MANDAT	(6)
# define OPT_OPTERROR	(7)

typedef enum
  {
    OPTION,
    MANDATORY
  }	t_opt_par;

typedef enum
  {
    ePCHAR = 0,
    eINT = 1,
    eFLOAT = 2,
    N_OPT_TYPES = 3
  }	t_opt_type;

typedef struct	s_getopt_init
{
}		t_getopt_init;

typedef struct	s_opt_v
{
  char		opt;
  char		set;
  t_opt_par	par;
  t_opt_type	type;
  int		(*func)(char *, void *);
  char		var[];
}		t_opt_v;

typedef struct	s_getopt
{
  t_object	__obj__;
  t_vector	*opts;
  char		*optstr;
  size_t	n_set;
  size_t	n_mandat;
  size_t	mandat_found;
}		t_getopt;

t_getopt	*t_getopt_new(t_getopt_init var);

int	opt_register(t_getopt *opt, char c, t_opt_type type, t_opt_par par);
int	opt_getvar(t_getopt *opt, char c, void *var);

int	opt_parse_params(t_getopt *opt, int ac, char **av);

#endif /* !GET_OPT_H_ */
