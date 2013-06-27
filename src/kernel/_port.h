/** Machine specific stuff
 * */
#ifndef __KERNEL__PORT_H__
#define __KERNEL__PORT_H__
//-----------------------------------------------------------------------------
#define RPUSH(r) asm volatile ("push r"#r);
#define RPOP(r) asm volatile ("pop r"#r);
#define port_SAVE_CONTEXT() \
	RPUSH(0);asm volatile ("in r0,__SREG__");RPUSH(0);\
	RPUSH(31);RPUSH(30);RPUSH(29);RPUSH(28);RPUSH(27);RPUSH(26);RPUSH(25);\
	RPUSH(24);RPUSH(23);RPUSH(22);RPUSH(21);RPUSH(20);RPUSH(19);RPUSH(18);\
	RPUSH(17);RPUSH(16);RPUSH(15);RPUSH(14);RPUSH(13);RPUSH(12);RPUSH(11);\
	RPUSH(10);RPUSH(9);RPUSH(8);RPUSH(7);RPUSH(6);RPUSH(5);RPUSH(4);\
	RPUSH(3);RPUSH(2);RPUSH(1);asm volatile("clr r1");\
	taskInfo.current->sp = (uint8_t *)(SP);
#define port_RESTORE_CONTEXT() \
	SP = (uint16_t)(taskInfo.current->sp);\
	RPOP(1);RPOP(2);RPOP(3);RPOP(4);RPOP(5);RPOP(6);RPOP(7);RPOP(8);\
	RPOP(9);RPOP(10);RPOP(11);RPOP(12);RPOP(13);RPOP(14);RPOP(15);RPOP(16);\
	RPOP(17);RPOP(18);RPOP(19);RPOP(20);RPOP(21);RPOP(22);RPOP(23);RPOP(24);\
	RPOP(25);RPOP(26);RPOP(27);RPOP(28);RPOP(29);RPOP(30);RPOP(31);\
	RPOP(0);asm volatile ("out __SREG__,r0");RPOP(0);
//-----------------------------------------------------------------------------
#endif
