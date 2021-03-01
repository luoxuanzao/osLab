
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_TASKS];

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK	task_table[NR_TASKS] = {{ReadA, STACK_SIZE_READA, "ReadA"},
					{ReadB, STACK_SIZE_READB, "ReadB"},
					{ReadC, STACK_SIZE_READC, "ReadC"},
                    {WriteD, STACK_SIZE_WRITED, "WriteD"},
                    {WriteE, STACK_SIZE_WRITEE, "WriteE"},
                    {ProcessF, STACK_SIZE_PROCESSF, "ProcessF"},
                    
                    };

PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_print_str, sys_sleep,sys_p,sys_v};

