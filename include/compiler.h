#ifndef COMPILER_H_
# define COMPILER_H_

# include <stdint.h>

# include "new.h"
# include "map.h"

# include "parser.h"

typedef unsigned char bytecode;

# define CODE_REALLOC_SIZE	(1024)

/*
** The VM will stop when it encounter this opcode.
** It's like abort() or exit() without return value
*/
# define OP_HALT		(0xFF)

/*
** The code size will start at size code_size
** The size will grow by CODE_REALLOC_SIZE if
** there are no more place (end reached)
*/
typedef struct	compiler_init
{
  size_t	code_size;
}		compiler_init;

/*
** The opcodes map contains :
** -> key = parser "opcode"
** -> value = function that "compile" the code (opcode_func)
*/
typedef struct	compiler
{
  t_object	__obj__;
  size_t	code_size;
  size_t	actual_code_size;
  bytecode	*code;
  bytecode	*current;
  bytecode	*end;
  t_map		*opcodes;
}		compiler;

typedef int (*opcode_func)(compiler *c, node *n);

compiler	*compiler_new(compiler_init var);

/*
** Used to push the data into the code.
*/
bytecode	*bc_push(compiler *c, void *data, size_t size);

/*
** Used to skip bytecode size "size"
** (for making jumps, size = 4)
*/
bytecode	*bc_hole(compiler *c, size_t size);

/*
** Used to insert the value of the difference
** between src's and dst's addresses. Used to
** make jumps.
*/
bytecode	*bc_push_diff(compiler *c, bytecode *src, bytecode *dest);

/*
** Compile the code to make bytecode from the AST
** built by the parser. The function takes the
** code kind and then check in the map if it exists.
** If a code is unknown, the function returns -1, else 0
** Predefined parser codes are :
**  - PROG (program beginning)
**  - EMPTY (leaf, does nothing)
*/
int		bc_compile(compiler *c, node *n);

#endif /* !COMPILER_H_ */
