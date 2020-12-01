/* Program: Simple OS experiment for 8051 uC.
			(it supports only 2 tasks, main thread and sub thread)
   Auther: 	Sandeep Verma
   Date: 	14-01-18
*/

#pragma SRC

/* These are the registers which will be saved on every context change.
	 When ISR is called PC of running context is saved first (push PC),
	 which is not memtioned below (output of assambler).

  PUSH     ACC(0xE0)
  PUSH     B(0xF0)
  PUSH     DP0H(0x83)
  PUSH     DP0L(0x82)
  PUSH     PSW(0xD0)
  PUSH     0x00
  PUSH     0x01
  PUSH     0x02
  PUSH     0x03
  PUSH     0x04
  PUSH     0x05
  PUSH     0x06
  PUSH     0x07
	
	Including of PC and above register total 15 bytes register is saved as context.
	(see struct context)
*/

struct context {
	char c[15];
}c1, c2;
 char temp;
 char pc1, pc2;

void start_task(void (*cb)(void)) 
{
	int _cb = (int)cb;
	/* c[13] and c[14] stores PC, Rest all registers will be 0 */
	c2.c[13] = (char)*((char *)&_cb); //0x00;
	c2.c[14] = (char)*(((char *)&_cb)+1); //0x03;
}
void change_context()
{
	char i;
	__asm POP pc1;	// POP and store PC (because of this function)
	__asm POP pc2;	// its a 2 byte PC 
	
	/* Save the ruuning context to context1 */
	for (i=0; i<15; i++) {	
		__asm POP temp;
		c1.c[i] = temp;
	}
	
	/* Push the stored context from context2 and copy context1 to context2 */
	for (i=14; i>=0; i--) {
		temp = c2.c[i];
		c2.c[i] = c1.c[i];
		__asm PUSH temp;
	}
	__asm PUSH pc2;	//PUSH the PC so that after returning from this function 
	__asm PUSH pc1; // control goes to proper location
}

