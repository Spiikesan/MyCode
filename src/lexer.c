#include "lexer.h"

static void	lexer_clear_sym(lexer *l)
{
  free(l->symbol_value);
  free(l->symbol);
  l->symbol_value = NULL;
  l->symbol_value_size = 0;
  l->symbol = NULL;
}

void	lexer_del(void *p)
{
  if (p)
    lexer_clear_sym((lexer *)p);
}

lexer	*lexer_new(lexer_init var)
{
  lexer	*l;

  if (var.stream == NULL ||
      (l = newObject(lexer, &lexer_del)) == NULL ||
      (l->validors = new(t_vector)) == NULL)
    return (NULL);
  l->stream = var.stream;
  l->act = ' ';
  return (l);
}

char	lexer_peek(lexer *l)
{
  /*  return (*l->steam->_IO_write_ptr) */;
  return (l->act);
}

char	lexer_next(lexer *l)
{
  return ((l->act = getc(l->stream)));
}

char	*lexer_sym(lexer *l, size_t sym_size, void *sym_value, const char *symname)
{
  lexer_clear_sym(l);
  assert(l->symbol = strdup(symname));
  if (sym_size != 0 && sym_value != NULL)
    {
      assert(l->symbol_value = malloc(sym_size));
      memcpy(l->symbol_value, sym_value, sym_size);
      l->symbol_value_size = sym_size;
    }
  return (l->symbol);
}

char	*valid_char(lexer *l)
{
  char	ch;

  ch = lexer_peek(l);
  while (ch == ' ' || ch == '\n')
    ch = lexer_next(l);
  switch (ch)
    {
    case EOF: case '#': lexer_next(l); printf("\nEnd of program input\n"); return lexer_sym(l, 0, NULL, "EOI");
    case '{': lexer_next(l); return lexer_sym(l, 0, NULL, "LBRA");
    case '}': lexer_next(l); return lexer_sym(l, 0, NULL, "RBRA");
    case '(': lexer_next(l); return lexer_sym(l, 0, NULL, "LPAR");
    case ')': lexer_next(l); return lexer_sym(l, 0, NULL, "RPAR");
    case '+': lexer_next(l); return lexer_sym(l, 0, NULL, "PLUS");
    case '-': lexer_next(l); return lexer_sym(l, 0, NULL, "MINUS");
    case '<': lexer_next(l); return lexer_sym(l, 0, NULL, "LESS");
    case '>': lexer_next(l); return lexer_sym(l, 0, NULL, "MORE");
    case ';': lexer_next(l); return lexer_sym(l, 0, NULL, "SEMI");
    case '=': lexer_next(l); return lexer_sym(l, 0, NULL, "EQUAL");
    case ',': lexer_next(l); return lexer_sym(l, 0, NULL, "COMMA");
    }
  return NULL;
};

char	*valid_num(lexer *l) /* missing overflow check (assert int_val > 0 ?) */
{
  const char	*symname = "INT";
  int	int_val;
  char	ch = lexer_peek(l);

  if (ch >= '0' && ch <= '9')
    {
      int_val = 0;
      while (ch >= '0' && ch <= '9')
	{
	  int_val = int_val*10 + (ch - '0');
	  ch = lexer_next(l);
	}
      return (lexer_sym(l, sizeof(int_val), &int_val, symname));
    }
  return (NULL);
}

char	*valid_id(lexer *l)
{
  const char const *syscalls[] = {
    "read",
    "write",
    NULL
  };
  const struct {
    char *key;
    char *sym;
  } keywords[] = {
    {"do", "DO_SYM"},
    {"else", "ELSE_SYM"},
    {"if", "IF_SYM"},
    {"while", "WHILE_SYM"},
    {NULL, NULL}
  };
  const char	*symname = "ID";
  char		ch = lexer_peek(l);
  char		id_name[MAX_ID_SIZE];
  size_t	i;

  if (ch >= 'a' && ch <= 'z')
    {
      i = 0;
      while (((ch >= 'a' && ch <= 'z') || ch == '_') && i < sizeof(id_name))
	{
	  id_name[i++] = ch;
	  ch = lexer_next(l);
	}
      if ((ch >= 'a' && ch <= 'z') || ch == '_')
	return (NULL); /* Need implementation of built-in error syntax gestion */
      id_name[i] = 0;
      for (i = 0; syscalls[i] != NULL; ++i) {
	if (strcmp(id_name, syscalls[i]) == 0) {
	  return (lexer_sym(l, strlen(id_name) + 1, id_name, "SYSCALL"));
	}
      }
      for (i = 0; keywords[i].key != NULL; ++i) {
	if (strcmp(id_name, keywords[i].key) == 0) {
	  return (lexer_sym(l, 0, NULL, keywords[i].sym));
	}
      }
      return (lexer_sym(l, strlen(id_name) + 1, id_name, symname));
    }
  return (NULL);
}

char		*get_next_symbol(lexer *l)
{
  char		*sym;

  /* Temporary */
  if (l->validors->size == 0) {
    vector_push_back(l->validors, valid_char);
    vector_push_back(l->validors, valid_num);
    vector_push_back(l->validors, valid_id);
  }

  FOREACH(lexer_validation_function, func, l->validors)
    if ((sym = func(l)) != NULL)
      return (sym);
  return (NULL);
}
