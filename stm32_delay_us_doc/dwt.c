#ifndef __DWT_H_
#define __DWT_H_
#include "stm32f4xx.h"

#define  DWT_CR      *(volatile uint32_t *)0xE0001000

#define  DWT_CYCCNT  *(volatile uint32_t *)0xE0001004

#define  DEM_CR      *(volatile uint32_t *)0xE000EDFC

#define  DEM_CR_TRCENA                   (1 << 24)

#define  DWT_CR_CYCCNTENA                (1 <<  0)

#define delayms(msec)         delayus(msec*1000)

void dwt_delay_init(uint32_t clk);
void delay_us(uint32_t usec);
void delay_ms(__IO uint32_t nTime);
#endif

#include "dwt.h"

static uint32_t cpuclkfeq;

void dwt_delay_init(uint32_t clk)
{
    cpuclkfeq = clk;
    DEM_CR         |=  DEM_CR_TRCENA;
    DWT_CYCCNT      = 0u;
    DWT_CR         |= DWT_CR_CYCCNTENA;
}

void delay_us(uint32_t usec)

{
    uint32_t startts, endts, ts;
	
    startts = DWT_CYCCNT;

    ts =  usec * (cpuclkfeq / (1000 * 1000));
    endts = startts + ts;
    if(endts > startts)
    {
        while(DWT_CYCCNT < endts);
    }
    else
    {
        while(DWT_CYCCNT > endts);
        while(DWT_CYCCNT < endts);
    }
}

void delay_ms(__IO uint32_t nTime)
{
    delay_us(1000 * nTime);
}


/*2*/
#include "dwt.h"
 
#define DWT_CR *(uint32_t *)0xE0001000
#define DWT_CYCCNT *(uint32_t *)0xE0001004
#define DEM_CR *(uint32_t *)0xE000EDFC
#define DEM_CR_TRCENA (1 << 24)
#define DWT_CR_CYCCNTENA (1 << 0)
 
/* 初始化时间戳 */
 
void CPU_TS_TmrInit(void)
 
{
 
/* 使能DWT外设 */
 
DEM_CR |= (uint32_t)DEM_CR_TRCENA;
 
/* DWT CYCCNT寄存器计数清0 */
 
DWT_CYCCNT = (uint32_t)0u;
 
/* 使能Cortex-M3 DWT CYCCNT寄存器 */
 
DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
 
} 
 
uint32_t OS_TS_GET(void)
 
{
	uint32_t _get_ts;
	
	uint32_t _ts;
	
	static uint32_t _ts_bak;    /* 时间戳备份 */
 
	_get_ts = DWT_CYCCNT;
 
	if(_get_ts < _ts_bak)
		{
			/* 做溢出修正 */
			_ts = 0XFFFFFFFF - _ts_bak + _get_ts;
			
			/* 加上上次数据 即可求出本次时间差*/
			_ts = _ts+_ts_bak;
		}
	else
		{
			/* 正常情况 */
			_ts = _get_ts;
		}
		
		_ts_bak = _ts;
		
		return _ts;
} 


//dwt.h
#ifndef _DWT_H
#define _DWT_H
#include "stm32f10x.h"
 
void CPU_TS_TmrInit(void);
uint32_t OS_TS_GET(void);
 
#endifk 
