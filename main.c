/* Program: Basic kernel with RR scheduler for 8051 uC.
   Auther:  Sandeep Verma
   Date:    14-01-18
*/

#include<reg51.h>

extern void register_task(void (*cb)(void));
extern void run_scheduler(void);

sbit LED_0 = P1^0;
sbit LED_1 = P1^1;
sbit LED_2 = P1^2;

void task_0(void)
{
	while(1) {
		/* Task to be executed in loop. Currently blinking LED_0 */
		LED_0 = ~LED_0;
	}
}

void task_1(void)
{
	while(1) {
		/* Task to be executed in loop. Currently blinking LED_1 */
		LED_1 = ~LED_1;
	}
}

void task_2(void)
{
	while(1) {
		/* Task to be executed in loop. Currently blinking LED_2 */
		LED_2 = ~LED_2;
	}
}

int main()
{
	register_task(&task_0);
	register_task(&task_1);
	register_task(&task_2);
  	run_scheduler();
	return 0;
}

