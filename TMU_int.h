/*
 * TMU_int.h
 *
 *  Created on: May 13, 2022
 *      Author: mabda
 */

#ifndef TMU_INT_H_
#define TMU_INT_H_


#define READY	50
#define SUSPEND 55
#define KILLED	60


void TMU_voidInit(void);

void TMU_voidCreateTask(void(*pfun)(void*),void * parameter, u16 priodicity, u8 priority, u8 offset);


void TMU_voidStartSchedular(void);


void TMU_voidSuspend(u8 priority);

void TMU_voidResume(u8 priority);

void TMU_voidDelete(u8 priority);


#endif /* TMU_INT_H_ */
