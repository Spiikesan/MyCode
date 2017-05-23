#ifndef OPT_TYPE_H
# define OPT_TYPE_H

# ifndef GET_OPT_H_
#  error "Never use opt_types.h directly; include get_opt.h instead."
# endif

# include <stdlib.h>

extern int	(*const opt_func[])(char *opt, void *type);

extern size_t const	opt_size[];

#endif /* !OPT_TYPE_H */
