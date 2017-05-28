#include <stdio.h>
#include "get_opt.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include "foreach.h"

int	printerror(char *msg, int return_value)
{
  fprintf(stderr, "ERROR: %s\n", msg);
  return (return_value);
}

void	*syntax_error(char *msg)
{
  if (msg)
    fprintf(stderr, "Syntax error : %s\n", msg);
  else
    fprintf(stderr, "Syntax error\n");
  return (NULL);
}

node *comma_expr(parser *p);
node *paren_expr(parser *p);

/* Parser functions */

int	get_var_id(node *n, char *var_name)
{
  int	max = -1;
  long int	act;

  if (!n)
    return 0;
  if ((act = MGET(long int, n->symbols, var_name)) != -1)
    return (act);
  return (max);
}

node *term(parser *p)  /* <term> ::= <id> | <int> | <paren_expr> | <id> "(" <comma_expr> ? ")" */
{
  node *t, *x = NULL;
  long int	i;

  if (sym_is(p, "SYSCALL"))
  {
    if (p->lexer->symbol_value != NULL) /* if the name of the syscal is get */
      {
	x = new(node, "SYS_CALL", p->lexer->symbol_value_size, p->lexer->symbol_value);
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
          return (syntax_error("Expected ')'"));
        next_sym(p);
      }
      else
        return (syntax_error("Expected '('"));
      return x;
    }
  }

  else if (sym_is(p, "ID")) //It's a variable/symbol name
    {
      i = get_var_id(p->root, (char *)p->lexer->symbol_value);
      if (i == -1)
	{
	  i = p->root->symbols->size;
	  map_add(p->root->symbols, strdup((char *)p->lexer->symbol_value), (void *)i);
	}
      x = new(node, "VAR", sizeof(i), &i);
      next_sym(p);
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
  node *t, *x = NULL;

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
  node *t, *x = NULL;

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
  node *t, *x = NULL;
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
  node *t, *x = NULL;

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
  node *x = NULL;
  if (sym_is(p, "LPAR"))
    next_sym(p);
  else
    return (syntax_error("Expected '('"));
  if ((x = expr(p)) == NULL)
      return (NULL);
  if (sym_is(p, "RPAR"))
    next_sym(p);
  else
    return (syntax_error("Expected ')'"));
  return x;
}

node	*statement(parser *p)
{
  node *t, *x = NULL;
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
      return (syntax_error("'while' keyword expected"));
    if ((t = paren_expr(p)) == NULL)
      return (NULL);
    node_add_child(x, t);
    if (sym_is(p, "SEMI"))
      next_sym(p);
    else
      return (syntax_error("';' expected"));
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
      return (syntax_error("';' expected"));
  }
  return x;
}

int	program(parser *p)
{
  node *n = NULL;
  next_sym(p);
  if ((n = statement(p)) != NULL)
  {
      node_add_child(p->root, n);
      return (0);
  }
  return (-1);
}

void	print_ast(node *n, int i)
{
  if (!n)
    return ;
  if (node_is(n, "ID") || node_is(n, "SYS_CALL"))
    printf("%.*sNode '%s' (%s)\n", i * 2, "                          ", n->kind, n->value ? (char *)n->value : 0);
  else
    printf("%.*sNode '%s' (%d)\n", i * 2, "                          ", n->kind, n->value ? *(int *)n->value : 0);
  FOREACH(node *, tmp, n->children)
    print_ast(tmp, i + 1);
};

enum {
    IFETCH = 0x00,
    ISTORE = 0x01,
    IPUSH = 0x02,
    IPOP = 0x03,
    IADD = 0x04,
    ISUB = 0x05,
    ILT = 0x06,
    IGT = 0x07,
    JZ = 0x08,
    JNZ = 0x09,
    JMP = 0x0A,
    CALL = 0x0B,
};

/* Compiler functions */

static int comp_VAR(compiler *c, node *n)
{
  bytecode bc = IFETCH;

  bc_push(c, &bc, sizeof(bc));
  bc_push(c, n->value, sizeof(bytecode));
  return (0);
}

static int comp_CST(compiler *c, node *n)
{
  bytecode bc = IPUSH;

  bc_push(c, &bc, sizeof(bc));
  bc_push(c, n->value, n->value_size);
  return (0);
}

static int comp_ADD(compiler *c, node *n)
{
  bytecode bc = IADD;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  return (0);
}

static int comp_SUB(compiler *c, node *n)
{
  bytecode bc = ISUB;
  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  return (0);
}

static int comp_CSV(compiler *c, node *n)
{
  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  return (0);
}

static int comp_SYS_CALL(compiler *c, node *n)
{
  const char const *syscalls[] = {
    "write",
    "read",
    NULL
  };
  bytecode bc = CALL;
  int	i;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  for (i = 0; syscalls[i] != NULL; ++i)
    if (strcmp(n->value, syscalls[i]) == 0)
      break;
  if (syscalls[i] == NULL)
    return (-1);
  bc_push(c, &i, sizeof(bytecode));
  return (0);
}

static int comp_LT(compiler *c, node *n)
{
  bytecode bc = ILT;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  return (0);
}

static int comp_GT(compiler *c, node *n)
{
  bytecode bc = IGT;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  return (0);
}

static int comp_SET(compiler *c, node *n)
{
  bytecode bc = ISTORE;
  node	*o1;

  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  o1 = VGETP(node *, n->children, 0);
  bc_push(c, o1->value, sizeof(bytecode));
  return (0);
}

static int comp_IF1(compiler *c, node *n)
{
  bytecode bc = JZ;
  bytecode *p1;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  p1 = bc_hole(c, sizeof(int32_t));
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push_diff(c, p1, c->current);
  return (0);
}

static int comp_IF2(compiler *c, node *n)
{
  bytecode bc = JZ;
  bytecode *p1, *p2;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  p1 = bc_hole(c, sizeof(int32_t));
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc = JMP;
  bc_push(c, &bc, sizeof(bc));
  p2 = bc_hole(c, sizeof(int32_t));
  bc_push_diff(c, p1, c->current);
  if (bc_compile(c, VGETP(node *, n->children, 2)) == -1)
    return (-1);
  bc_push_diff(c, p2, c->current);
  return (0);
}

static int comp_WHILE(compiler *c, node *n)
{
  bytecode bc = JZ;
  bytecode *p1, *p2;

  p1 = c->current;
  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  p2 = bc_hole(c, sizeof(int32_t));
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc = JMP;
  bc_push(c, &bc, sizeof(bc));
  bc_push_diff(c, bc_hole(c, sizeof(int32_t)), p1);
  bc_push_diff(c, p2, c->current);
  return (0);
}

static int comp_DO(compiler *c, node *n)
{
  bytecode bc = JNZ;
  bytecode *p1;

  p1 = c->current;
  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  bc_push_diff(c, bc_hole(c, sizeof(int32_t)), p1);
  return (0);
}

static int comp_SEQ(compiler *c, node *n)
{
  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  if (bc_compile(c, VGETP(node *, n->children, 1)) == -1)
    return (-1);
  return (0);
}

static int comp_EXPR(compiler *c, node *n)
{
  bytecode bc = IPOP;

  if (bc_compile(c, VGETP(node *, n->children, 0)) == -1)
    return (-1);
  bc_push(c, &bc, sizeof(bc));
  return (0);
}

/* Virtual machine */

static int	vm_IFETCH(vm *v)
{
  *v->sp = v->registers[(int)*v->pc];
  v->sp++;
  v->pc++;
  return (0);
}

static int	vm_ISTORE(vm *v)
{
  v->registers[(int)*v->pc] = v->sp[-1];
  v->pc++;
  return (0);
}

static int	vm_IPUSH(vm *v)
{
  memcpy(v->sp, v->pc, sizeof(*v->sp));
  v->sp++;
  v->pc += sizeof(*v->sp);
  return (0);
}

static int	vm_IPOP(vm *v)
{
  --v->sp;
  return (0);
}

static int	vm_IADD(vm *v)
{
  v->sp[-2] = v->sp[-2] + v->sp[-1];
  --v->sp;
  return (0);
}

static int	vm_ISUB(vm *v)
{
  v->sp[-2] = v->sp[-2] - v->sp[-1];
  --v->sp;
  return (0);
}

static int	vm_ILT(vm *v)
{
  v->sp[-2] = v->sp[-2] < v->sp[-1];
  --v->sp;
  return (0);
}

static int	vm_IGT(vm *v)
{
  v->sp[-2] = v->sp[-2] > v->sp[-1];
  --v->sp;
  return (0);
}

static int	vm_JMP(vm *v)
{
  int32_t	jmp;

  memcpy(&jmp, v->pc, sizeof(jmp));
  v->pc += jmp;
  return (0);
}

static int	vm_JZ(vm *v)
{
  int32_t	jmp;

  memcpy(&jmp, v->pc, sizeof(jmp));
  --v->sp;
  if (*v->sp == 0)
    v->pc += jmp;
  else
    v->pc += sizeof(jmp);
  return (0);
}

static int	vm_JNZ(vm *v)
{
  int32_t	jmp;

  memcpy(&jmp, v->pc, sizeof(jmp));
  --v->sp;
  if (*v->sp != 0)
    v->pc += jmp;
  else
    v->pc += sizeof(jmp);
  return (0);
}

/* syscalls */
void sys_read(int **params)
{
  **params = getc(stdin);
  (*params)++;
}

/* syscalls */
void sys_write(int **params)
{
  putc((*params)[-1], stdout);
}

static int	vm_CALL(vm *v)
{
  size_t	i;
  void (*sys_funcs[])(int **) = {
    sys_write,
    sys_read
  };
  const size_t size = sizeof(sys_funcs) / sizeof(*sys_funcs);

  i = (int)*v->pc;
  v->pc++;
  if (i < size)
    sys_funcs[i](&v->sp);
  else
    return (-1);
  return (0);
}

void dump_hex(compiler *c)
{
  size_t i;
  size_t dump_size = ((c->actual_code_size / 8) + (c->actual_code_size % 8 != 0));

  printf("Dump of the code program (%lu code bytes, %lu dump bytes):\n", c->actual_code_size, dump_size * 8);

  for (i = 0; i < dump_size; ++i)
    {
      printf("0x%04lX: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n", i * 8,
	     (unsigned char)c->code[i * 8],
	     (unsigned char)c->code[i * 8 + 1],
	     (unsigned char)c->code[i * 8 + 2],
	     (unsigned char)c->code[i * 8 + 3],
	     (unsigned char)c->code[i * 8 + 4],
	     (unsigned char)c->code[i * 8 + 5],
	     (unsigned char)c->code[i * 8 + 6],
	     (unsigned char)c->code[i * 8 + 7]);
    }
  printf("\n");
}

#define ENUM_TO_STR(v) printf("0x%02X = %s | ", v, #v)

size_t	cpos(compiler *c, bytecode *pos)
{
  return (pos - c->code);
}

void dump_asm(compiler *c)
{
    int32_t	val;
    bytecode *pc = c->code;

    ENUM_TO_STR(IFETCH);
    ENUM_TO_STR(ISTORE);
    ENUM_TO_STR(IPUSH);
    ENUM_TO_STR(IPOP);
    ENUM_TO_STR(IADD);
    ENUM_TO_STR(ISUB);
    ENUM_TO_STR(ILT);
    ENUM_TO_STR(IGT);
    ENUM_TO_STR(JZ);
    ENUM_TO_STR(JNZ);
    ENUM_TO_STR(JMP);
    ENUM_TO_STR(CALL);
    ENUM_TO_STR(HALT);
    printf("\n");
    again:
    switch (*pc++)
    {
    case IFETCH: printf("0x%04lX: fetch reg[0x%02X]\n", cpos(c, pc)-1, *pc); pc++; goto again;
    case ISTORE: printf("0x%04lX: store reg[0x%02X]\n", cpos(c, pc)-1, *pc); pc++;                               goto again;
    case IPUSH : memcpy(&val, pc, sizeof(val)); printf("0x%04lX: push 0x%08X // %d\n", cpos(c, pc)-1, val, val); pc+=sizeof(val); goto again;
    case IPOP  : printf("0x%04lX: pop\n", cpos(c, pc)-1);                                                                           goto again;
    case IADD  : printf("0x%04lX: add\n", cpos(c, pc)-1);                                                                           goto again;
    case ISUB  : printf("0x%04lX: sub\n", cpos(c, pc)-1);                                                                           goto again;
    case ILT   : printf("0x%04lX: complt\n", cpos(c, pc)-1);                                                                        goto again;
    case IGT   : printf("0x%04lX: compgt\n", cpos(c, pc)-1);                                                                        goto again;
    case JMP   : memcpy(&val, pc, sizeof(val)); printf("0x%04lX: jmp 0x%08lX\n", cpos(c, pc)-1, cpos(c, pc) + val); pc+=sizeof(val); goto again;
    case JZ    : memcpy(&val, pc, sizeof(val)); printf("0x%04lX: jz 0x%08lX\n", cpos(c, pc)-1, cpos(c, pc) + val); pc+=sizeof(val); goto again;
    case JNZ   : memcpy(&val, pc, sizeof(val)); printf("0x%04lX: jnz 0x%08lX\n", cpos(c, pc)-1, cpos(c, pc) + val); pc+=sizeof(val); goto again;
    case CALL  : printf("0x%04lX: call 0x%02X\n", cpos(c, pc)-1, *pc); pc++; goto again;
    case HALT  : printf("0x%04lX: halt\n\n", cpos(c, pc)-1); break;
    }
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
  /* printf("AST\n"); */
  /* print_ast(par->root, 0); */

  if ((comp = new(compiler, 1024)) == NULL)
    return (printerror("compiler is null !", -1));

  if (map_add(comp->opcodes, "VAR", comp_VAR) != MAP_NOERR ||
      map_add(comp->opcodes, "CST", comp_CST) != MAP_NOERR ||
      map_add(comp->opcodes, "ADD", comp_ADD) != MAP_NOERR ||
      map_add(comp->opcodes, "SUB", comp_SUB) != MAP_NOERR ||
      map_add(comp->opcodes, "CSV", comp_CSV) != MAP_NOERR ||
      map_add(comp->opcodes, "SYS_CALL", comp_SYS_CALL) != MAP_NOERR ||
      map_add(comp->opcodes, "LT", comp_LT) != MAP_NOERR ||
      map_add(comp->opcodes, "GT", comp_GT) != MAP_NOERR ||
      map_add(comp->opcodes, "SET", comp_SET) != MAP_NOERR ||
      map_add(comp->opcodes, "IF1", comp_IF1) != MAP_NOERR ||
      map_add(comp->opcodes, "IF2", comp_IF2) != MAP_NOERR ||
      map_add(comp->opcodes, "WHILE", comp_WHILE) != MAP_NOERR ||
      map_add(comp->opcodes, "DO", comp_DO) != MAP_NOERR ||
      map_add(comp->opcodes, "SEQ", comp_SEQ) != MAP_NOERR ||
      map_add(comp->opcodes, "EXPR", comp_EXPR) != MAP_NOERR)
    return (printerror("Compilation functions registration error", -1));

  if (bc_compile(comp, par->root) == -1)
    return (printerror("-1: compilation error", -1));

  /* dump_hex(comp); */
  /* dump_asm(comp); */

  if ((virmac = new(vm, comp->code, comp->code_size)) == NULL)
    return (printerror("vm is null !", -1));

  if (vm_register_opcode(virmac, IFETCH, vm_IFETCH) == -1 ||
      vm_register_opcode(virmac, ISTORE, vm_ISTORE) == -1 ||
      vm_register_opcode(virmac, IPUSH, vm_IPUSH) == -1 ||
      vm_register_opcode(virmac, IPOP, vm_IPOP) == -1 ||
      vm_register_opcode(virmac, IADD, vm_IADD) == -1 ||
      vm_register_opcode(virmac, ISUB, vm_ISUB) == -1 ||
      vm_register_opcode(virmac, ILT, vm_ILT) == -1 ||
      vm_register_opcode(virmac, IGT, vm_IGT) == -1 ||
      vm_register_opcode(virmac, JMP, vm_JMP) == -1 ||
      vm_register_opcode(virmac, JZ, vm_JZ) == -1 ||
      vm_register_opcode(virmac, JNZ, vm_JNZ) == -1 ||
      vm_register_opcode(virmac, CALL, vm_CALL) == -1)
    return (printerror("Running functions registration error", -1));

  printf("Running...\n");
  if (run(virmac) == -1)
    return (printerror("-1: run error", -1));

  delete(virmac);
  delete(comp);
  delete(par);
  delete(lex);
  return (0);
}
