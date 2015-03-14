#include "delay.h"

void Delay(uint32_t i)	//	 V_is_working  #don't touch!
{
	for(; i > 1; i --)
	{
		__NOP();
	}
}
