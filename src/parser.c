#include "parser.h"

parser	*parser_new(UNUSED parser_init var)
{
  parser	*p;

  if (var.lexer == NULL ||
      (p = newObject(parser)) == NULL ||
      (p->root = new(node, "PROG")) == NULL)
    return (NULL);
  p->lexer = var.lexer;
  return (p);
}

void	node_del(void *p)
{
  node	*n;
  if (p)
    {
      n = p;
      free(n->kind);
      free(n->value);
    }
}

node	*node_new(node_init var)
{
  node	*n;

  if (var.kind == NULL ||
      (n = newObject(node, node_del)) == NULL ||
      (n->children = new(t_vector)) == NULL ||
      (n->symbols = new(t_vector)) == NULL ||
      (n->kind = strdup(var.kind)) == NULL)
    return (NULL);
  if (var.value != NULL && var.value_size != 0)
    {
      if ((n->value = malloc(var.value_size)) == NULL)
	return (NULL);;
      memcpy(n->value, var.value, var.value_size);
    }
  return (n);
}

void	node_add_child(node *n, node *c)
{
  if (n && c)
    {
      vector_push_back(n->children, c);
    }
}