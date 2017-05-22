#ifndef LEXER_H_
# define LEXER_H_

# include <assert.h>
# include <stdio.h>

# include "new.h"
# include "vector.h"
# include "util.h"
# include "foreach.h"

/*
enum { DO_SYM, ELSE_SYM, IF_SYM, WHILE_SYM, CALL_SYM, LBRA, RBRA, LPAR, RPAR,
PLUS, MINUS, LESS, MORE, SEMI, EQUAL, INT, ID, COMMA, EOI };
*/

# define MAX_ID_SIZE	(126)

typedef struct	lexer_init
{
  FILE		*stream;
}		lexer_init;

typedef struct
{
  t_object	__obj__;
  t_vector	*validors;
  FILE		*stream;
  void		*symbol_value;
  char		*symbol;
  char		act;
}		lexer;

/* Used to validate a sequence. Return the symbol name if found. */
typedef char *(*lexer_validation_function)(lexer *);

lexer	*lexer_new(lexer_init var);

char	*lexer_sym(lexer *l, size_t sym_size, void *sym_value, const char *symname);
char	lexer_peek(lexer *);
char	lexer_next(lexer *);
char	*get_next_symbol(lexer *);

#endif /* !LEXER_H_ */
