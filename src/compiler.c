#include "compiler.h"

void	compiler_del(void *p)
{
  if (p)
    free(((compiler *)p)->code);
}

static int	string_test(const void *a, const void *b)
{
  if (!a || !b)
    return (-1);
  return (strcmp((char *)a,(char *)b));
}

static	int	PROG_op(compiler *c, node *n)
{
  printf("Beginning program compilation\n");
  if (n->children->size > 0)
    if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
      return (-1);
  bc_push(c, OP_HALT);
  return (0);
}

static	int	EMPTY_op(UNUSED compiler *c, UNUSED node *n)
{
  printf("Empty op creation\n");
  return (0);
}

compiler	*compiler_new(compiler_init var)
{
  compiler	*c;

  if ((c = newObject(compiler, compiler_del)) == NULL ||
      (c->opcodes = new(t_map, string_test)) == NULL)
    return (NULL);
  if (map_add(c->opcodes, "PROG", PROG_op) != MAP_NOERR ||
      map_add(c->opcodes, "EMPTY", EMPTY_op) != MAP_NOERR)
    return (NULL);
  if (var.code_size == 0)
    var.code_size = CODE_REALLOC_SIZE;
  c->code_size = var.code_size;
  if ((c->code = malloc(c->code_size)) == NULL)
    return (NULL);
  c->current = c->code;
  c->end = c->code + c->code_size;
  return (c);
}

/*
** Get the func that correspond to the kind of the node
** that be able to compile the code of that node
*/
int	bc_compile(compiler *c, node *n)
{
  opcode_func	func;

  func = MGET(opcode_func, c->opcodes, n->kind);
  if (func != NULL)
    return (func(c, n));
  return (-1);
}

bytecode	*bc_push(compiler *c, bytecode bc)
{
  *c->current++ = bc;
  return (c->current);
}

/* return ((c->current += size) - size); */
bytecode	*bc_hole(compiler *c, size_t size)
{
  bytecode	*tmp;

  tmp = c->current;
  c->current += size;
  return (tmp);
}

bytecode	*bc_push_diff(UNUSED compiler *c, bytecode *src, bytecode *dest)
{
  int32_t	diff;

  diff = dest - src;
  memcpy(src, &diff, sizeof(diff));
  return (dest);
}
