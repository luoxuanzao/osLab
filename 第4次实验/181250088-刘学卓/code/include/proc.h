
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


typedef struct s_stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;		/* ┓						│			*/
	u32	fs;		/* ┃						│			*/
	u32	es;		/* ┃						│			*/
	u32	ds;		/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()				│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;		/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;


typedef struct s_proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */

        int ticks;                 /* remained ticks */
        int priority;

		int wake_time;
		int  wait_for_sem;

	u32 pid;                   /* process id passed in from MM */
	char p_name[16];           /* name of the process */
}PROCESS;

typedef struct s_task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
}TASK;

# define  SEMAPHORE_SIZE 20
typedef struct semaphore {
	int value,start,end;
	PROCESS* list[SEMAPHORE_SIZE];
}SEMAPHORE;


/* Number of tasks */
#define NR_TASKS	6

/* stacks of tasks */
#define STACK_SIZE_READA	0x8000
#define STACK_SIZE_READB	0x8000
#define STACK_SIZE_READC	0x8000
#define STACK_SIZE_WRITED	0x8000
#define STACK_SIZE_WRITEE   0x8000
#define STACK_SIZE_PROCESSF	0x8000

#define STACK_SIZE_TOTAL	(STACK_SIZE_READA + \
				STACK_SIZE_READB + \
				STACK_SIZE_READC + STACK_SIZE_WRITED + STACK_SIZE_WRITEE + STACK_SIZE_PROCESSF)


PUBLIC void sys_p(SEMAPHORE* s);
PUBLIC void sys_v(SEMAPHORE* s);
PUBLIC void sys_print_str(char * str);


PUBLIC void my_p(SEMAPHORE* s);
PUBLIC void my_v(SEMAPHORE* s);

PUBLIC  void init_sem(SEMAPHORE*s, int val);
PUBLIC void sys_sleep(int  milli_seconds);
