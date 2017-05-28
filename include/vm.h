#ifndef VM_H_
# define VM_H_

# include <stdlib.h>
# include <stdint.h>

# include "new.h"

# include "compiler.h"

# define HALT (0xFF)

# define DEFAULT_STACK_SIZE (1024)

typedef struct	vm_init
{
  bytecode	*program;
  size_t	program_size;
  size_t	stack_size;
}		vm_init;

/*
** sp is the stack pointer
** pc is the program counter
** opcodes = map<bytecode *, vm_opcode_func>
*/
typedef struct	vm
{
  t_object	__obj__;
  t_map		*opcodes;
  size_t	stack_size;
  int32_t	*stack;
  int32_t	*sp;
  bytecode	*program;
  size_t	program_size;
  bytecode	*pc;
  int32_t	registers[32];
}		vm;

typedef int (*vm_opcode_func)(vm *v);

vm	*vm_new(vm_init var);

int	run(vm *v);

int	vm_register_opcode(vm *v, bytecode b, vm_opcode_func func);

#endif /* !VM_H_ */
