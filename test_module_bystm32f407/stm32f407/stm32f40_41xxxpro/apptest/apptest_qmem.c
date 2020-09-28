#include "basictype.h"
#include "contiki.h"
#include "sysprintf.h"
#include "qmemmang.h"




PROCESS(q_mem_test, "q_mem");
PROCESS(crc_8_p, "crc8"); //x^8+x^2+x+1  init = 0x41


PROCESS_THREAD(q_mem_test, ev, data)
{
	static struct etimer _et;
	static u_char *p[32] = {NULL};
	static u_char i = 0;
	
	PROCESS_BEGIN();
	
	while(1)
	{	
		etimer_set(&_et, 100);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
		if (i < 32)
		{
			p[i] = qMalloc(16);
			memset(p[i], i, 16);
		}

		if (i < 32)
		{
			XPRINTF((6, "p[%d] = %08x\r\n", i, p[i]));
			MEM_DUMP(6, "qmem", p[i], 16);
			//qFree(p[i]);
			i++;
		}		

		if (i >= 32)
		{
			for (i = 0; i < 32; i++)
			{
				qFree(p[i]);
			}
			i = 0;
		}
	}
	PROCESS_END();
}



unsigned char  crc_8(unsigned char *const pstr,int length){
	unsigned char* p= pstr;
	int len = length;
	unsigned char crc = 0x41;//根据自己实际情况或者手册中初值
	
	unsigned char i = 0;
	while(len--){
		crc ^= *p++;
		for(i = 8 ; i > 0 ; --i){
			if(crc & 0x80){					//判断最高位是不是为1 
				crc =  (crc << 1)^ 0x07; 	// 最高位为1，不需要异或，往左移一位，然后与0x31异或 
			}else{
				crc = (crc << 1); 
			} 
		} 
	} 
	return crc;
}


PROCESS_THREAD(crc_8_p, ev, data)
{
	static struct etimer _et;
	static const u_char cd = 0x01;
	u_char crcd = 0;
	
	PROCESS_BEGIN();
	
	while(1)
	{	
		etimer_set(&_et, 1000);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
		crcd = crc_8((unsigned char * const)&cd, 1);
		XPRINTF((0, "CRCD= %02x\r\n", crcd));
	}
	PROCESS_END();
}



void app_test_qmem(void)
{
	process_start(&crc_8_p, NULL);
	process_start(&q_mem_test, NULL);
}
