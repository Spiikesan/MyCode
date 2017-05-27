#ifndef PARSER_H_
# define PARSER_H_

# include <stdint.h>

# include "new.h"
# include "vector.h"

# include "lexer.h"

typedef struct	node_init
{
  char		*kind;
  size_t	value_size;
  void		*value;
}		node_init;

typedef struct	node
{
  t_object	__obj__;
  char		*kind;
  void		*value;
  size_t	value_size;
  t_vector	*children;
  t_vector	*symbols;
}		node;

typedef struct	parser_init
{
  lexer		*lexer;
}		parser_init;

typedef struct	parser
{
  t_object	__obj__;
  lexer		*lexer;
  node		*root;
}		parser;

/*
** Must build an AST (for example, with descent-recursive parsing)
*/

parser	*parser_new(parser_init var);
node	*node_new(node_init var);
int 	sym_is(parser *p, char *sym);
int     node_is(node *n, char *kind);
void    next_sym(parser *p);
void  node_rename(node *n, char *name);


void	node_add_child(node *n, node *c);

#endif /* !PARSER_H_ */
