#include "vm.h"

static void	vm_del(void *p)
{
  vm	*v;

  if (p)
    {
      v = p;
      free(v->stack);
      free(v->program);
      FOREACH(t_pair *, p, v->opcodes)
	free(p->first);
      /* May NOT bug with GC*/
      delete(v->opcodes);
    }
}

static int	bytecode_compare(const void *a, const void *b)
{
  bytecode	ba;
  bytecode	bb;

  if (!a || !b)
    return (-1);
  ba = *((bytecode *)a);
  bb = *((bytecode *)b);
  return ((int)(ba - bb));
}

static int	VM_halt(UNUSED vm *v)
{
  printf("Halting virtual machine...\n");
  return ((int)HALT);
}

vm	*vm_new(vm_init var)
{
  vm	*v;

  if (var.program == NULL || var.program_size == 0 ||
      (v = newObject(vm, vm_del)) == NULL ||
      (v->opcodes = new(t_map, bytecode_compare)) == NULL)
    return (NULL);
  if (vm_register_opcode(v, HALT, VM_halt) == -1)
    return (NULL);
  v->stack_size = (var.stack_size ? var.stack_size : DEFAULT_STACK_SIZE);
  v->program_size = var.program_size;
  if ((v->stack = calloc(v->stack_size, sizeof(*v->stack))) == NULL ||
      (v->program = calloc(v->program_size, sizeof(bytecode))) == NULL)
    return (NULL);
  memcpy(v->program, var.program, v->program_size);
  v->sp = v->stack;
  v->pc = v->program;
  return (v);
}

int	run(vm *v)
{
  vm_opcode_func func;
  int		ret;

  while ((func = MGET(vm_opcode_func, v->opcodes, v->pc++)) != NULL &&
	 (size_t)(v->pc - v->program) < v->program_size)
    {
      ret = func(v);
      if (ret == -1)
	return (-1);
      else if (ret == HALT)
	return (0);
    }
  return (0);
}

int	vm_register_opcode(vm *v, bytecode b, vm_opcode_func func)
{
  bytecode	*bc;

  if (!v)
    return (-1);
  if ((bc = calloc(1, sizeof(bytecode))) == NULL)
    return (-1);
  memcpy(bc, &b, sizeof(bytecode));
  if (map_add(v->opcodes, bc, func) != MAP_NOERR)
    return (-1);
  return (0);
}
