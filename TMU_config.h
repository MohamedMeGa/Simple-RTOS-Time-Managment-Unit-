/*
 * TMU_congif.h
 *
 *  Created on: May 13, 2022
 *      Author: mabda
 */

#ifndef TMU_CONFIG_H_
#define TMU_CONFIG_H_

/*=========== Enter your OS TICK TIME in ms ======*/
/*===== it must not exceed 1500 ms =====*/
/*this number must cast to unsigned long number*/
#define OS_TICK		10UL

/*	Select your Timer Channel [TIMER0 / TIMER1 / TIMER2]	*/
#define TIMER_CH	TIMER0

#define CLOCK_FREQ 	16000UL


/**	Enter the number of your tasks in your system 	*/
/**	Do not Exceed 10 Tasks							*/
#define MAX_NUM_TASKS			2

#endif /* TMU_CONFIG_H_ */
