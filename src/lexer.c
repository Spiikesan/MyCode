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
	return (NULL); /* Need implementation of error syntax gestion */
      id_name[i] = '\0';
      return (lexer_sym(l, strlen(id_name), id_name, symname));
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

/* void next_sym() */
/* { */
/*     again: switch (ch) */
/*     { */
/*         case ' ': case '\n': next_ch(); goto again; */
/*         case EOF: sym = EOI; printf("\nEnd of input\n"); break; */
/*         case '#': sym = EOI; getchar(); printf("End of input\n"); break; */
/*         case '{': next_ch(); sym = LBRA; break; */
/*         case '}': next_ch(); sym = RBRA; break; */
/*         case '(': next_ch(); sym = LPAR; break; */
/*         case ')': next_ch(); sym = RPAR; break; */
/*         case '+': next_ch(); sym = PLUS; break; */
/*         case '-': next_ch(); sym = MINUS; break; */
/*         case '<': next_ch(); sym = LESS; break; */
/*         case '>': next_ch(); sym = MORE; break; */
/*         case ';': next_ch(); sym = SEMI; break; */
/*         case '=': next_ch(); sym = EQUAL; break; */
/*         case ',': next_ch(); sym = COMMA; break; */
/*         default: */
/*         if (ch >= '0' && ch <= '9') */
/*         { */
/* 	  int_val = 0; /\* missing overflow check *\/ */
/* 	  while (ch >= '0' && ch <= '9') */
/*             { */
/*                 int_val = int_val*10 + (ch - '0'); next_ch(); */
/*             } */
/*             sym = INT; */
/*         } */
/*         else if (ch >= 'a' && ch <= 'z') */
/*         { */
/*             int i = 0; */
/*             while (((ch >= 'a' && ch <= 'z') || ch == '_') && i < sizeof(id_name)) */
/*             { */
/*                 id_name[i++] = ch; */
/*                 next_ch(); */
/*             } */
/*             if ((ch >= 'a' && ch <= 'z') || ch == '_') /\* more than sizeof(id_name) characters ? *\/ */
/*                 syntax_error("Too much characters in identifier (max is 126)"); */
/*             id_name[i] = '\0'; */
/*             sym = 0; */
/*             while (words[sym] != NULL && strcmp(words[sym], id_name) != 0) */
/*                 sym++; */
/*             if (words[sym] == NULL) */
/*             { */
/*                 //printf("Requesting ID symbol '%s'\n", id_name); */
/*                 sym = ID;  /\* Adding named IDs (and not only a...z ids that are actually registers) that can contains up to 128 chars a...b and _*\/ */
/*             } */
/*         } */
/*         else */
/*             syntax_error("Unknown character"); */
/*     } */
/* } */
