#include "stm32f30x.h"

#include "types_P.h"
#include "led_blink.h"
#include "delay.h"

int main(void)
{
	ledInit();

    while(1)
    {
    	Delay(80000);	//100ms
    	GreenHighLedChangeState();
    }
}
