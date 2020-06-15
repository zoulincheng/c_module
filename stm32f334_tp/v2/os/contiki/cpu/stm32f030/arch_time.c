#include "arch_time.h"
/*software delay*/
/*
need to adjust by mcu work freq.
now freq is 48000000Hz
*/
void delayUs(int nUs)
{
	int i = 0; 
	int j = 0;

	for (j = 0; j < nUs; j++)
	{
		for (i = 0; i < 50; i++)
		{
			
		}
	}
}
