/* Program: Micro scheduler for 8051.
   Auther: 	Sandeep Verma
   Date: 		14-01-18
*/

#include<reg51.h>

typedef void (*task_t)(void);

typedef struct {
	union {
		struct cpu_reg {
			/* These CPU registers will be saved on every context change */
			char acc;
			char b;
			char dph;
			char dpl;
			char psw;
			char r00;
			char r01;
			char r02;
			char r03;
			char r04;
			char r05;
			char r06;
			char r07;
			char pc_lsb;
			char pc_msb;
		} cpu_reg;
		char reg[15];
	} u;
	task_t fun;
} pcb_t;

#define MAX_TASK_SUPPORTED 4

typedef struct scheduler {
	pcb_t context[MAX_TASK_SUPPORTED];
	int max_task_registered;
	int curr_task_idx;
} sceduler_t;

sceduler_t sch;

void enable_timer0()
{
	/* Timer expiry after 32ms (when 12MHz Xtal Oscillator used) */
	TH0 = 0xFF;
	TL0 = 0xE0;
	TMOD = (TMOD & 0xF0) | 0x01;  /* Set T/C0 Mode */
	ET0 = 1;                      /* Enable Timer 0 Interrupts */
	TR0 = 1;                      /* Start Timer 0 */
	EA = 1;                       /* Global Interrupt Enable */
}


void register_task(task_t cb) 
{
	int _cb = (int)cb;
 
	if (sch.max_task_registered > MAX_TASK_SUPPORTED)
		return;
	
	sch.context[sch.max_task_registered].fun = cb;
	sch.context[sch.max_task_registered].u.cpu_reg.pc_lsb = (char)*((char *)&_cb);
	sch.context[sch.max_task_registered].u.cpu_reg.pc_msb = (char)*(((char *)&_cb)+1);
	sch.max_task_registered++;
}


int pick_next_task()
{
	/* pick next task in RR (Round Robin) fashion */
	int next_task_idx = sch.curr_task_idx + 1;
	if (next_task_idx >= sch.max_task_registered)
		next_task_idx = 0;
	
	return next_task_idx;
}

void run_scheduler()
{
	/* start the periodic timer.
	 * ISR will change the PCB 
	 */
	enable_timer0();
	
	/* start 1st registered task in main routine 
	 * once ISR is called next task will start executing.
	 */
	sch.context[0].fun();
}

char pc_l8, pc_u8;
char temp;
void change_pcb(int next_task_idx)
{
	char i;
  
	/* POP and store return address first. 
	 * This is required because ISR is calling change_pcb function and return address got pushed on stack.
	 * To get running context register values, first need to pop recently pushed values because of LIFO system.
	 */ 
	
	__asm POP pc_l8;
	__asm POP pc_u8;
	
	/* When ISR gets called, running context (CPU registers) get pushed to stack. 
	 * Need to pop and store it on software data structure 
	 */
	for (i=0; i<=14; i++) {	
		__asm POP temp;
		sch.context[sch.curr_task_idx].u.reg[i] = temp;
	}
	
	/* Push the saved context (which needs to be run next) to stack */
	for (i=14; i>=0; i--) {
		temp = sch.context[next_task_idx].u.reg[i];
		__asm PUSH temp;
	}
	
	sch.curr_task_idx = next_task_idx;
	
	/* store the return address back, so upon completion of this funtion, PC gets next instruction to executes */
	__asm PUSH pc_u8;	
	__asm PUSH pc_l8; 
}

void timer0_ISR (void) interrupt 1
{	
	int next_task_idx = pick_next_task();
	change_pcb(next_task_idx);
	
	/* Reschedule timer after 32ms (when 12MHz Xtal Oscillator used) */
	TH0 = 0xFF;
	TL0 = 0xE0;
}


