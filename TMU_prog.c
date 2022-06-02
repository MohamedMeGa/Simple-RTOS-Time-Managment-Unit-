/*
 * TMU_prog.c
 *
 *  Created on: May 13, 2022
 *      Author: mabda
 */
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "AVR_REG.h"

#include "TMU_int.h"
#include "TMU_config.h"
#include "TMU_private.h"


static u8 Num_CTC	=0;
static u8 CTC_count	=0;

static u16 Num_ovf	=0;
static u16 OVF_count=0;

static u8 preload;

static TCB_t All_Tasks[ MAX_NUM_TASKS ];
static u32 OS_Tick_Total = 0;

void TMU_voidInit(void)
{
	#if TIMER_CH == TIMER0

		/*	CTC, dis connect	*/
		TCCR0 = 0x0D;
		#if OS_TICK > 0 && OS_TICK <=16

			OCR0 = (u8) (OS_TICK * CLOCK_FREQ) / (1024UL);
			Num_CTC = 1;
			CTC_count = Num_CTC;
			TIMSK |= (1 << 1);

		#elif OS_TICK > 16 && OS_TICK <= 1500

			u8 max;
			for(int i=16; i>0; i--)
			{
				if((OS_TICK % i)==0)
				{
					max = i;
					break;
				}
			}

			if(max > 1)
			{
				OCR0 = (u8) (max * OS_TICK) / (1024UL);
				Num_CTC = OS_TICK / max;
				CTC_count = Num_CTC;
				TIMSK |= (1 << 1);
			}
			else if(max == 1)
			{
				//OVF, dis co0, 1024
				TCCR0 = 0x05;
				f32 ovf = (OS_TICK/16.384);

				if(ovf - (u32)ovf > 0)
				{
					Num_ovf = (u16)ovf +1;
					OVF_count = Num_ovf;
					preload = (u8)(256 * (1 - (ovf - (u32)ovf)));
					TCNT0 = preload;
					TIMSK |= (1 << 0);
				}else
				{
					Num_ovf = (u16)ovf;
					TIMSK |= (1 << 0);
				}
			}

		#else
			#error "OS Tick is Bigger than 1500 ms"
		#endif

	#elif TIMER_CH == TIMER1
			/**	CTC - Dis Oc1a OC1b - 1024	*/
			TCCR1A = 0x00;
			TCCR1B = 0x0D;
			#if OS_TICK > 0 && OS_TICK <= 1500
				u16 counts = (u16) ((OS_TICK * CLOCK_FREQ) / 1024UL);
				OCR1A = (u8)(counts>>8);
				OCR1B = (u8)(counts);
				TIMSK |= (1 << 4);
			#endif

	#elif TIMER_CH == TIMER2
		//CTC, dis connect
		TCCR2 = 0x0F;
		#if OS_TICK > 0 && OS_TICK <=16

			OCR2 = (u8) (OS_TICK * CLOCK_FREQ) / (1024UL);
			Num_CTC = 1;
			CTC_count = Num_CTC;
			TIMSK |= (1 << 7);

		#elif OS_TICK > 16 && OS_TICK <= 1500
			u8 max;
			for(int i=16; i>0; i--)
			{
				if((OS_TICK % i)==0)
				{
					max = i;
					break;
				}
			}

			if(max > 1)
			{
				OCR2 = (u8) (max * CLOCK_FREQ) / (1024UL);
				Num_CTC = OS_TICK / max;
				CTC_count = Num_CTC;
				TIMSK |= (1 << 7);
			}
			else if(max == 1)
			{
				//OVF, dis co2, 1024
				TCCR2 = 0x07;
				f32 ovf = (OS_TICK/16.384);

				if(ovf - (u32)ovf > 0)
				{
					Num_ovf = (u16)ovf +1;
					OVF_count = Num_ovf;
					preload = (u8)(256 * (1 - (ovf - (u32)ovf)));
					TCNT2 = preload;
					TIMSK |= (1 << 6);

				}else
				{
					Num_ovf = (u16)ovf;
					TIMSK |= (1 << 6);
				}
			}

			#else
				#error "OS Tick is Bigger than 1500 ms"
			#endif

	#else
		#error "Timer Channel config is Wrong"
	#endif

}


void TMU_voidCreateTask(void(*pfun)(void*),void * parameter, u16 priodicity, u8 priority, u8 offset)
{
	#if MAX_NUM_TASKS > 0 && MAX_NUM_TASKS > 10
		#error	"Max Number of Tasks Must be Not exceed 10, it must be below 10"
	#endif
	if(pfun != NULL && priodicity > 0 && priority<MAX_NUM_TASKS)
	{
		All_Tasks[priority].fun 		= pfun;
		All_Tasks[priority].parameter	= parameter;
		All_Tasks[priority].priodicity 	= priodicity;
		All_Tasks[priority].state 		= READY;
		All_Tasks[priority].offset 		= offset;
	}
}



void TMU_voidStartSchedular(void)
{
	u32 Temp_Ticks = 0;
	asm("SEI");
	while(1)
	{
		if(Temp_Ticks > OS_Tick_Total)
		{
			Temp_Ticks = OS_Tick_Total;

			//Descending [highest == Max-1]
			//for(s8 i= MAX_NUM_TASKS; i>=0; i--)
			//Ascending [highest == 0]
			for(u8 i=0; i<MAX_NUM_TASKS; i++)
			{
				if(All_Tasks[i].fun != NULL)
				{
					if((
							((OS_Tick_Total % All_Tasks[i].priodicity == 0) && OS_Tick_Total>= All_Tasks[i].offset)
							|| OS_Tick_Total == All_Tasks[i].offset)
							&& All_Tasks[i].state == READY)
					{
						//Dispatcher
						All_Tasks[i].fun(All_Tasks[i].parameter);
					}
				}
			}
		}

	}
}



void TMU_voidSuspend(u8 priority)
{
	if(priority < MAX_NUM_TASKS)
		All_Tasks[priority].state = SUSPEND;
}

void TMU_voidResume(u8 priority)
{
	if(priority < MAX_NUM_TASKS && All_Tasks[priority].state == SUSPEND)
		All_Tasks[priority].state = READY;
}

void TMU_voidDelete(u8 priority)
{
	if(priority < MAX_NUM_TASKS){
			All_Tasks[priority].state = KILLED;
			All_Tasks[priority].fun = NULL;
	}
}




/**				Timer Interrupt			*/
#if TIMER_CH == TIMER0
	void __vector_10(void) __attribute__((signal));
	void __vector_10(void)
	{
		CTC_count--;
		if(CTC_count == 0)
		{
			OS_Tick_Total++;


			CTC_count = Num_CTC;
		}
	}



	void __vector_11(void) __attribute__((signal));
	void __vector_11(void)
	{
		OVF_count--;
		if(OVF_count == 0)
		{
			TCNT0 = preload;
			OS_Tick_Total++;

			OVF_count = Num_ovf;
		}
	}

#elif TIMER_CH == TIMER1
	void __vector_7(void) __attribute__((signal));
	void __vector_7(void)
	{
		OS_Tick_Total++;

	}






#elif TIMER_CH == TIMER2
	void __vector_4(void) __attribute__((signal));
	void __vector_4(void)
	{
		CTC_count--;
		if(CTC_count == 0)
		{
			OS_Tick_Total++;

			CTC_count = Num_CTC;
		}
	}

	void __vector_5(void) __attribute__((signal));
	void __vector_5(void)
	{
		OVF_count--;
		if(OVF_count == 0)
		{
			TCNT2 = preload;
			OS_Tick_Total++;

			OVF_count = Num_ovf;
		}
	}
#endif
