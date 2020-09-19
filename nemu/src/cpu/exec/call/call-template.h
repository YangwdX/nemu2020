#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(){
	//push(eip)
	//read operand address
	//eip += relative address
	reg_l(R_ESP) -= DATA_BYTE;
	swaddr_write(reg_l(R_ESP), 4 , cpu.eip + DATA_BYTE);
	DATA_TYPE_S displacement = op_src->val;
	print_asm("call %x",cpu.eip + 1 + displacement + displacement);
	cpu.eip += displacement;
}

make_instr_helper(i);

/*make_helper (concat(call_i_,SUFFIX)){
	int len = concat(decode_i_,SUFFIX)(eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;
	swaddr_write(reg_l(R_ESP), 4 , cpu.eip + len);
	DATA_TYPE_S displacement = op_src->val;
	print_asm("call %x",cpu.eip + 1 + len + displacement);
	cpu.eip += displacement;
	return len + 1;
}*/
#include "cpu/exec/template-end.h"
