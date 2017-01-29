#include "TIMEOUT.h"
#include "timer16.h"

void TIMEOUT_Init(void)
{
	Time_t *p_timeout_time;
	
	p_timeout_time = TIMEOUT_GetTime();
	TIM16_Init(); 
	p_timeout_time->seconds = 0;
	p_timeout_time->minutes = MINUTES_TIMEOUT_DEFAULT;
}

void TIMEOUT_Start(uint16_t minutes, uint8_t seconds)
{
	Time_t *p_timeout_time;
	
	p_timeout_time = TIMEOUT_GetTime();
	p_timeout_time->minutes = minutes;
	p_timeout_time->seconds = seconds;  
	TIM16_Start();  
}

void TIMEOUT_Stop(void)
{
	TIM16_Stop();    
	TIM16_Reset();   
}

uint8_t TIMEOUT_HasRanOut(void)
{
	Time_t *p_timeout_time;

	p_timeout_time = TIMEOUT_GetTime();
	if ((p_timeout_time->minutes == 0) && (p_timeout_time->seconds == 0))
		return 1;

	return 0;
}

Time_t * TIMEOUT_GetTime(void)
{
	static Time_t timeout_time;
	return &timeout_time;
}

void TIMEOUT_Update(void)
{
	Time_t *p_timeout_time;

	p_timeout_time = TIMEOUT_GetTime();

	if (p_timeout_time->seconds > 0)
		p_timeout_time->seconds--;
	else
		if (p_timeout_time->minutes > 0)
		{
			p_timeout_time->seconds = 59;
			p_timeout_time->minutes--;
		}
		else
			TIMEOUT_Stop();
}

