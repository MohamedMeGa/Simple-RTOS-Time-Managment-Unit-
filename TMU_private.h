/*
 * TMU_private.h
 *
 *  Created on: May 13, 2022
 *      Author: mabda
 */

#ifndef TMU_PRIVATE_H_
#define TMU_PRIVATE_H_

#define TIMER0		0
#define TIMER1		1
#define TIMER2		2

typedef struct{
	void (*fun)(void*);
	void * parameter;
	u16 priodicity;
	u8 state;
	u8 offset;
}TCB_t;

#define READY	50
#define SUSPEND 55
#define KILLED	60

#endif /* TMU_PRIVATE_H_ */
