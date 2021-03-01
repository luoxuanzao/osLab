
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
		PROCESS* p = p_proc_ready;//当前进程
		while(1){
			int t = get_ticks();
			p++;
			if(p >= proc_table + NR_TASKS){
				p = proc_table;
			}
			if((!p->wait_for_sem == 1)&&p->wake_time<=t){
				p_proc_ready = p;
				break;
			}
		}


}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}



PUBLIC void sys_sleep(int  milli_seconds){
	p_proc_ready->wake_time = get_ticks()+milli_seconds/(1000/HZ);  // 修改当前进程的wake time
	
	schedule();
}

PUBLIC void sys_p(SEMAPHORE* s){
	s->value--;

	if(s->value<0){
		//阻塞该进程

		p_proc_ready->wait_for_sem = 1;
		s->list[s->end] = p_proc_ready;
		s->end = (s->end+1) % SEMAPHORE_SIZE;
		schedule();
	}

}

PUBLIC void sys_v(SEMAPHORE* s){
	s->value++;

	if(s->value<=0){
		s->list[s->start]->wait_for_sem = 0;
		s->start = (s->start+1)%SEMAPHORE_SIZE;
	}
}

PUBLIC void sys_print_str(char * str){
	disp_str(str);
}

PUBLIC  void init_sem(SEMAPHORE*s, int val){

	s->value = val;
	s->end = 0;
	s->start = 0;
}
