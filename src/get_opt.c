#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "get_opt.h"
#include "foreach.h"

void		opt_del(void *ptr)
{
  t_getopt	*g;

  if (!ptr)
    return ;
  g = ptr;
  FOREACH(t_opt_v *, o, g->opts)
    free(o);
  delete(g->opts);
  free(g->optstr);
}

t_getopt	*t_getopt_new(__attribute__((unused))t_getopt_init var)
{
  t_getopt	*g;

  if ((g = newObject(t_getopt, &opt_del)) == NULL ||
      (g->opts = new(t_vector)) == NULL)
    return (NULL);
  return (g);
}

int		opt_register(t_getopt *opt, char c, t_opt_type type, t_opt_par par)
{
  t_opt_v	*tmp;

  if (!opt || !isgraph(c) || type < 0 || type > N_OPT_TYPES)
    return (OPT_PARAMS);
  FOREACH(t_opt_v *, o, opt->opts)
    if (o->opt == c)
      return (OPT_EXIST);
  if ((tmp = malloc(sizeof(t_opt_v) + opt_size[type])) == NULL)
    return (OPT_FUNC);
  if (par == MANDATORY)
    ++opt->n_mandat;
  memset(tmp, 0, sizeof(t_opt_v) + opt_size[type]);
  tmp->opt = c;
  tmp->par = par;
  tmp->type = type;
  tmp->func = opt_func[type];
  if (vector_push_back(opt->opts, tmp) != VEC_NOERR ||
      (opt->optstr = realloc(opt->optstr, opt->opts->size * 2 + 1)) == NULL)
    {
      free(tmp);
      return (OPT_FUNC);
    }
  sprintf(&opt->optstr[(opt->opts->size - 1) * 2], "%c:", c);
  return (OPT_NOERR);
}

int		opt_getvar(t_getopt *opt, char c, void *var)
{
  t_opt_v	*tmp;

  tmp = NULL;
  if (!opt || !var)
    return (OPT_PARAMS);
  FOREACH(t_opt_v *, o, opt->opts)
    if (o->opt == c)
      {
	tmp = o;
	break;
      }
  if (!tmp)
    return (OPT_NOTFOUND);
  if (!tmp->set)
    return (OPT_NOTSET);
  memcpy(var, &tmp->var, opt_size[tmp->type]);
  return (OPT_NOERR);
}

int	opt_parse_params(t_getopt *opt, int ac, char **av)
{
  int	i;

  if (!opt || ac <= 1 || !av)
    return (OPT_PARAMS);
  for (opt->n_set = 0, opt->mandat_found = 0, i = 1; i < ac; ++i)
    {
      if (opt->opts->size == opt->n_set)
	return (opt->mandat_found == opt->n_mandat ? OPT_NOERR : OPT_MANDAT);
      while (i < ac - 1 && *av[i] != '-' && *av[i] != '/')
	++i;
      if (i < ac - 1)
	{
	  FOREACH(t_opt_v *, o, opt->opts)
	    if (i < ac - 1 && !o->set && o->opt == av[i][1] && ++o->set)
	      {
		++i;
		if (o->par == MANDATORY)
		  ++opt->mandat_found;
		if (o->func(av[i], &o->var) == -1)
		  return (OPT_OPTERROR);
		++opt->n_set;
	      }
	}
    }
  return (opt->mandat_found == opt->n_mandat ? OPT_NOERR : OPT_MANDAT);
}
