#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "get_opt.h"

size_t const	opt_size[] = {
  sizeof(char *),
  sizeof(int),
  sizeof(float)
};

static int	opt_pchar(char *opt, void *type)
{
  *(void **)type = opt;
  return (0);
}

static int	opt_int(char *opt, void *type)
{
  int		var;
  char		*endptr;

  var = (int)strtol(opt, &endptr, 0);
  if (opt == endptr || errno)
    return (-1);
  memcpy(type, &var, sizeof(var));
  return (0);
}

static int	opt_float(char *opt, void *type)
{
  float		var;
  char		*endptr;

  var = strtof(opt, &endptr);
  if (opt == endptr || errno)
    return (-1);
  memcpy(type, &var, sizeof(var));
  return (0);
}

int	(*const opt_func[])(char *opt, void *type) = {
  opt_pchar,
  opt_int,
  opt_float
};
