#include <stdio.h>
#include "get_opt.h"
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

node *comma_expr(parser *p);
node *paren_expr(parser *p);

node *term(parser *p)  /* <term> ::= <id> | <int> | <paren_expr> | <id> "(" <comma_expr> ? ")" */
{
  char  *syscalls[] = {
    "write",
    "read",
    NULL
  };
  node *t, *x;
  int i;
  
  if (sym_is(p, "ID"))
  {
    i = 0;
    /* check for syscall id */
    while (syscalls[i] != NULL && strcmp(syscalls[i], p->lexer->symbol_value) != 0)
      ++i;
    if (syscalls[i] != NULL) /* if it's a syscall */
    {
      //printf("it's a syscall !\n");
      x = new(node, "SYS_CALL", sizeof(i), &i);
      //x->val = i; /* call the n'th (i) syscall */
      next_sym(p);
      if (sym_is(p, "LPAR"))
      {
        next_sym(p);
        if (!sym_is(p, "RPAR"))
        {
          if ((t = comma_expr(p)) == NULL)
            return (NULL);
          node_add_child(x, t);
        }
        else
        {
          if ((t = new(node, "EMPTY")) == NULL)
            return (NULL);
          node_add_child(x, t);
        }
        if (!sym_is(p, "RPAR"))
          return (NULL); //syntax_error("Expected ')'");
        next_sym(p);
      }
      else
        return (NULL);
//        syntax_error("Expected '(' -> system call needs parameters");
      return x;
    }
    else //It's a variable/symbol name
    {
      // for (i = 0; i < sym_count; ++i)
      // {
        // if (!strcmp(sym_table[i].name, id_name)) // if the symbol already exists, we keep the index
        // {
          // // printf("symbol %s found (id=%d)\n", id_name, i);
          // x = new_node(var);
          // x->val = i;
          // next_sym();
          // return x;
        // }
      // }//we didn't find the element, so we create it if there is enough place
      // if (sym_count == (sizeof(sym_table) / sizeof(symbol)))
        // syntax_error("to much symbols, max is 32");
      // // printf("symbol %s not found. creating one (id=%d)\n", id_name, sym_count);
      // x = new_node(var);
      // strncpy(sym_table[sym_count].name, id_name, 126);
      // x->val=sym_count; //keep the index of the named symbol
      // ++sym_count;
      // next_sym();
      // /* if the next symbol is 'semi', then it's a use of an unknown symbol => syntax_error() */
      // if (sym == semi)
        // syntax_error("use of unknown symbol");
    }
  }
  else if (sym_is(p, "INT"))
  {
    if ((x = new(node, "CST", p->lexer->symbol_value_size, p->lexer->symbol_value)) == NULL)
      return (NULL);
    next_sym(p);
  }
  else
    return paren_expr(p);
  return x;
}

node *sum(parser *p)  /* <sum> ::= <term> | <sum> "+" <term> | <sum> "-" <term> */
{
  node *t, *x;
  
  if ((x = term(p)) == NULL)
    return (NULL);
  while (sym_is(p, "PLUS") || sym_is(p, "MINUS"))
  {
    t = x;
    if ((x = new(node, sym_is(p, "PLUS") ? "ADD" : "SUB")) == NULL)
      return (NULL);
    next_sym(p);
    node_add_child(x, t);
    if ((t = term(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
  }
  return x;
}

node *test(parser *p)  /* <test> ::= <sum> | <sum> ["<" | ">"] <sum> */
{
  node *t, *x;
  
  if ((x = sum(p)) == NULL)
    return (NULL);
  if (sym_is(p, "LESS") || sym_is(p, "MORE"))
  {
    t = x;
    if ((x = new(node, sym_is(p, "LESS") ? "LT" : "GT")) == NULL)
      return (NULL);
    next_sym(p);
    node_add_child(x, t);
    if ((t = sum(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
  }
  return x;
}

node *expr(parser *p)  /* <expr> ::= <test> | <id> "=" <expr> */
{
  node *t, *x;
  if (!sym_is(p, "ID"))
      return test(p);
  if ((x = test(p)) == NULL)
    return (NULL);
  if (node_is(x, "VAR") && sym_is(p, "EQUAL"))
  {
      t = x;
      if ((x = new(node, "SET")) == NULL)
        return (NULL);
      next_sym(p);
      node_add_child(x, t);
      if ((t = expr(p)) == NULL)
        return (NULL);
      node_add_child(x, t);
  }
  return x;
}

node *comma_expr(parser *p) /* <comma_expr> ::= <expr> [, <expr>] */
{
  node *t, *x;
  
  if ((x = expr(p)) == NULL)
      return (NULL);
  while (sym_is(p, "COMMA"))
  {
    t = x;
    if ((x = new(node, "CSV")) == NULL)
      return (NULL);
    next_sym(p);
    node_add_child(x, t);
    if ((t = expr(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
  }
  return x;
}

node *paren_expr(parser *p)  /* <paren_expr> ::= "(" <expr> ")" */
{
  node *x;
  if (sym_is(p, "LPAR"))
    next_sym(p);
  else
    return (NULL);
  if ((x = expr(p)) == NULL)
      return (NULL);
  if (sym_is(p, "RPAR"))
    next_sym(p);
  else
    return (NULL);
//    syntax_error("')' expected");
  return x;
}

node	*statement(parser *p)
{
//  node	*x = new(node, "EMPTY");
//  node_add_child(n, x);
  node *t, *x;
  if (sym_is(p, "IF_SYM"))  /* "if" <paren_expr> <statement> */
  {
    if ((x = new(node, "IF1")) == NULL)
      return (NULL);
    next_sym(p);
    if ((t = paren_expr(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if ((t = statement(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if (sym_is(p, "ELSE_SYM"))  /* ... "else" <statement> */
    {
      node_rename(x, "IF2");
      next_sym(p);
      if ((t = statement(p)) == NULL)
        return (NULL);
      node_add_child(x, t);
    }
  }
  else if (sym_is(p, "WHILE_SYM"))  /* "while" <paren_expr> <statement> */
  {
    if ((x = new(node, "WHILE")) == NULL)
      return (NULL);
        next_sym(p);
    if ((t = paren_expr(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if ((t = statement(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
  }
  else if (sym_is(p, "DO_SYM"))  /* "do" <statement> "while" <paren_expr> ";" */
  {
    if ((x = new(node, "DO")) == NULL)
      return (NULL);
    next_sym(p);
    if ((t = statement(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if (sym_is(p, "WHILE_SYM"))
      next_sym(p);
    else
      return (NULL);
//            syntax_error("'while' keyword expected");
    if ((t = paren_expr(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if (sym_is(p, "SEMI"))
      next_sym(p);
    else
      return (NULL);
//          syntax_error("';' expected");
  }
  else if (sym_is(p, "SEMI"))  /* ";" */
  {
    if ((x = new(node, "EMPTY")) == NULL)
      return (NULL);
    next_sym(p);
  }
  else if (sym_is(p, "LBRA"))  /* "{" { <statement> } "}" */
  {
    if ((x = new(node, "EMPTY")) == NULL)
      return (NULL);
    next_sym(p);
    while (!sym_is(p, "RBRA"))
    {
      t=x;
      if ((x=new(node, "SEQ")) == NULL)
        return (NULL);
      node_add_child(x, t);
      if ((t = statement(p)) == NULL)
        return (NULL);
      node_add_child(x, t);
    }
    next_sym(p);
  }
  else  /* <expr> ";" */
  {
    if ((x = new(node, "EXPR")) == NULL ||
        (t = expr(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if (sym_is(p, "SEMI"))
      next_sym(p);
    else
      return (NULL);
//            syntax_error("';' expected");
  }
  return x;
}

int	program(parser *p)
{
  node *n;
  lexer_next(p->lexer);
  if ((n = statement(p)) != NULL)
  {
      node_add_child(p->root, n);
      return (0);
  }
  return (-1);
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
