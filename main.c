/* Program: Simple OS experiment for 8051 uC.
			(it supports only 2 tasks, main thread and sub thread)
   Auther: 	Sandeep Verma
   Date: 	14-01-18
*/

#include<reg51.h>

extern void start_task(void (*cb)(void));
extern void change_context(void);

sbit LED_1 = P1^1;
sbit LED_2 = P1^2;

typedef void (*fun)(void);
static int overflow_count = 0;

int first = 1; 
int stack_p = 0;
int program_c = 0;

void timer0_ISR (void) interrupt 1
{	
	change_context();
}

void enable_timer0()
{
	TMOD = (TMOD & 0xF0) | 0x01;  /* Set T/C0 Mode */
	ET0 = 1;                      /* Enable Timer 0 Interrupts */
	TR0 = 1;                      /* Start Timer 0 Running */
	EA = 1;                       /* Global Interrupt Enable */
	
	TH0 = 0xFC;		/* Load higher 8-bit in TH0 */
	TL0 = 0x74;		/* Load lower 8-bit in TL0 */
}

void delay(unsigned int d)
{ 
  unsigned char i;
  for(;d>0;d--)
  {
   for(i=250;i>0; i--);
   for(i=254;i>0; i--);
  }
}

void thread_1()
{
	while(1) {
	LED_1 = ~LED_1;
	//delay(50);
	//LED_1 = ~LED_1;
	delay(50);
	}
}

void thread_2(void)
{
	while(1) {
	LED_2 = ~LED_2;
//	delay(50);
//	LED_2 = ~LED_2;
	delay(50);
	}
}

main()
{
	fun fun1;
	fun1 = thread_2;
	start_task(thread_1);
	enable_timer0();
	(*fun1)();
  while(1);
}

