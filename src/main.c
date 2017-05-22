#include <stdio.h>
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"

int	printerror(char *msg, int return_value)
{
  fprintf(stderr, "ERROR: %s\n", msg);
  return (return_value);
}

int	nothing(UNUSED parser *p, node *n)
{
  node	*x = new(node, "EMPTY");

  if (n == NULL)
    return (-1);
  node_add_child(n, x);
  return (0);
}

int	program(parser *p)
{
  return (nothing(p, p->root));
}

int	main(UNUSED int ac, UNUSED char *av[])
{
  /* The lexer will read from stdin */
  lexer *lex = NULL;
  parser *par = NULL;
  compiler *comp = NULL;
  vm	*virmac = NULL;


  if ((lex = new(lexer, stdin)) == NULL)
    return (printerror("lexer is null !", -1));
  if ((par = new(parser, lex)) == NULL)
    return (printerror("parser is null !", -1));
  program(par);
  if ((comp = new(compiler, 1024)) == NULL)
    return (printerror("compiler is null !", -1));
  if (bc_compile(comp, par->root) == -1)
    return (printerror("-1: compilation error", -1));
  if ((virmac = new(vm, comp->code, comp->code_size)) == NULL)
    return (printerror("vm is null !", -1));
  if (run(virmac) == -1)
    return (printerror("-1: run error", -1));

  delete(virmac);
  delete(comp);
  delete(par);
  delete(lex);
  return (0);
}
