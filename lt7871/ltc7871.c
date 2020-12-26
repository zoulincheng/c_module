#include "contiki.h"
#include "iodef.h"
#include "basictype.h"
#include "arch_spi.h"
#include "ltc7871.h"
#include "sysprintf.h"
#include "arch_dac.h"

//

static void swpower_pin_init(void);
static void obj_init(LTC7871_OBJ *pLtObj,const LTC7871_FUN *pfun, LTC7871_REG * preg, u_char mod);
static void set_fan(u_char flag);		//		//1 on, 0 off
static void swpower_set_mode(u_char flag, LTC7871_OBJ *pLtObj);
static void swpower_set_run(u_char flag, LTC7871_OBJ *pLtObj);
static void swpower_set_buck(u_char flag, LTC7871_OBJ *pLtObj);
static void swpower_set_mos_sw(u_char flag, LTC7871_OBJ *pLtObj);//MCU->not gate-> and gate->mos driver
static uint8_t swpower_read_fault(LTC7871_OBJ *pLtObj);
void lt7871_write_buffer( uint8_t addr, uint8_t *buffer, uint8_t size );
void lt7871_read_buffer( uint8_t addr, uint8_t *buffer, uint8_t size );
static int swpower_set_buckmode(LTC7871_OBJ *pLtObj);
static int swpower_set_boostmode(LTC7871_OBJ *pLtObj);
static int swpower_set_mode_continuous(LTC7871_OBJ *pLtObj);
static int swpower_set_mode_discontinuous(LTC7871_OBJ *pLtObj);
static int swpower_set_mos_close(LTC7871_OBJ *pLtObj);
static int swpower_set_mos_open(LTC7871_OBJ *pLtObj);
static int swpower_set_run_running(LTC7871_OBJ *pLtObj);
static int swpower_set_run_stop(LTC7871_OBJ *pLtObj);
static int swpower_set_current(int value);

int lt7871_set_fan_run(LTC7871_OBJ *pLtObj);
int lt7871_set_fan_stop(LTC7871_OBJ *pLtObj);


static void battery_pin_init(void);
static void battery_set_mode(u_char flag, LTC7871_OBJ *pLtObj);
static void battery_set_run(u_char flag, LTC7871_OBJ *pLtObj);
static void battery_set_buck(u_char flag, LTC7871_OBJ *pLtObj);
static void battery_set_mos_sw(u_char flag, LTC7871_OBJ *pLtObj);//MCU->not gate-> and gate->mos driver
static uint8_t battery_read_fault(LTC7871_OBJ *pLtObj);
void lt7871_write_buffer_battery( uint8_t addr, uint8_t *buffer, uint8_t size );
void lt7871_read_buffer_battery( uint8_t addr, uint8_t *buffer, uint8_t size );
static int battery_set_buckmode(LTC7871_OBJ *pLtObj);
static int battery_set_boostmode(LTC7871_OBJ *pLtObj);
static int battery_set_mode_continuous(LTC7871_OBJ *pLtObj);
static int battery_set_mos_close(LTC7871_OBJ *pLtObj);
static int battery_set_mos_open(LTC7871_OBJ *pLtObj);
static int battery_set_run_running(LTC7871_OBJ *pLtObj);
static int battery_set_run_stop(LTC7871_OBJ *pLtObj);
static int battery_set_current(int value);



/*
write:
For the serial port write sequence, the master calculates
the PEC byte for the address byte and data byte it sends
out
read:
For the serial port read sequence, the LTC7871 calculates
PEC byte for the received address byte and data byte it
sends out. 
*/

/*
BIT[6]= 1 sink current ï¿½ï¿½ï¿½ëµ½Ð¾Æ¬ï¿½ï¿½ï¿½ï¿½,  BIT[6]= 0 source current ï¿½ï¿½Ð¾Æ¬ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
index 0 -> 0 uA
index 1 -> -1 uA
*/
//start var define
static const u_char _sink_current[] = {
	0x00, 
	0x7f,0x7e,0x7d,0x7c,0x7b,0x7a,0x79,0x78,0x77,0x76,0x75,0x74,0x73,0x72,0x71,0x70,
	0x6f,0x6e,0x6d,0x6c,0x6b,0x6a,0x69,0x68,0x67,0x66,0x65,0x64,0x63,0x62,0x61,0x60,
	0x5f,0x5e,0x5d,0x5c,0x5b,0x5a,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,
	0x4f,0x4e,0x4d,0x4c,0x4b,0x4a,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40
};

/*
index 0 -> 0 uA
index 1 -> 1 uA
*/
static const u_char _source_current[] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f	
};




PROCESS(lt7871, "lt7871");
//
const LTC7871_FUN tgLtc7871_fun_swpower = {
	swpower_pin_init,
	obj_init,
	lt7871_write_buffer,
	lt7871_read_buffer,
	set_fan,
	swpower_set_mode,
	swpower_set_run,
	swpower_set_buck,
	swpower_set_mos_sw,
	swpower_read_fault,
	swpower_set_current,
};
const LTC7871_FUN tgLtc7871_fun_battery = {
	battery_pin_init,
	obj_init,
	lt7871_write_buffer_battery, 
	lt7871_read_buffer_battery,	 
	set_fan,
	battery_set_mode,
	battery_set_run,
	battery_set_buck,
	battery_set_mos_sw,
	battery_read_fault,
	battery_set_current,
};
//default value
LTC7871_REG tgLtc7871Reg_Swpower = {
	{MFR_FAULT, 			0x00},
	{MFR_OC_FAULT, 			0x00},
	{MFR_NOC_FAULT, 		0x00},
	{MFR_STATUS,			0x00},
	{MFR_CONFIG1,			0x00},
	{MFR_CONFIG2,			0x00},
	{MFR_CHIP_CTRL,			0x00},
	{MFR_IDAC_VLOW,			0x00},
	{MFR_IDAC_VHIGH,		0x00},
	{MFR_IDAC_SETCUR,		0x00},
	{MFR_SSFM,				0x00}
};

//default value
LTC7871_REG tgLtc7871Reg_Battery = {
	{MFR_FAULT, 			0x00},
	{MFR_OC_FAULT, 			0x00},
	{MFR_NOC_FAULT, 		0x00},
	{MFR_STATUS,			0x00},
	{MFR_CONFIG1,			0x00},
	{MFR_CONFIG2,			0x00},
	{MFR_CHIP_CTRL,			0x00},
	{MFR_IDAC_VLOW,			0x00},
	{MFR_IDAC_VHIGH,		0x00},
	{MFR_IDAC_SETCUR,		0x00},
	{MFR_SSFM,				0x00}
};


LTC7871_OBJ tgLtc7871Obj_Swpower = {0x00};
LTC7871_OBJ tgLtc7871Obj_Battery = {0x00};

//end var define

//ï¿½ï¿½ï¿½ï¿½ 1ï¿½ï¿½ï¿½Ø±ï¿½:0
#define RUNA(a)										\
	if(a)											\
		{GPIO_SetBits(GPIOC,GPIO_Pin_10);}			\
	else											\
		{GPIO_ResetBits(GPIOC,GPIO_Pin_10);}
        //ï¿½ï¿½ï¿½ï¿½ 1ï¿½ï¿½ï¿½Ø±ï¿½:0


 //ï¿½ï¿½ï¿½ 0 ï¿½ï¿½ï¿½Åµï¿½ 1
#define BUCK(a)										\
	if(a)											\
		{GPIO_SetBits(GPIOC,GPIO_Pin_11);}			\
	else											\
		{GPIO_ResetBits(GPIOC,GPIO_Pin_11);}
#define FAULTA(a)									\
	if(a)											\
		{GPIO_SetBits(GPIOD,GPIO_Pin_2);}			\
	else											\
		{GPIO_ResetBits(GPIOD,GPIO_Pin_2);}
        
#define RUNB(a)										\
	if(a)											\
		{GPIO_SetBits(GPIOB,GPIO_Pin_5);}			\
	else											\
		{GPIO_ResetBits(GPIOB,GPIO_Pin_5);}

void initCtrl(void)
{
	GPIO_InitTypeDef		stGpioInit;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOD, ENABLE);  
	stGpioInit.GPIO_Pin= GPIO_Pin_15|GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &stGpioInit);	
	stGpioInit.GPIO_Pin= GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_10;
	GPIO_Init(GPIOC, &stGpioInit);	
	stGpioInit.GPIO_Pin= GPIO_Pin_2;
	GPIO_Init(GPIOD, &stGpioInit);	
	RUNA(1);
	RUNB(1);
	BUCK(1);
}

//swpower module function
static void swpower_pin_init(void)
{
	GPIO_InitTypeDef		stGpioInit;

	//init ltc7871_swpower pin init run, buck, mode, fault
	stGpioInit.GPIO_Pin= LT7871_A_RUN_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LT7871_A_RUN_PORT, &stGpioInit);
	LT7871_A_RUN_SET(0);//stop run when init

	stGpioInit.GPIO_Pin= LT7871_A_BUCK_PIN;
	GPIO_Init(LT7871_A_BUCK_PORT, &stGpioInit);
	LT7871_A_BUCK_SET(1);//init sw power module in buck mode, 

	stGpioInit.GPIO_Pin= LT7871_A_MODE_PIN;
	GPIO_Init(LT7871_A_MODE_PORT, &stGpioInit);
	LT7871_A_MODE_SET(0);//set float mode pin

	//control mos pin
	stGpioInit.GPIO_Pin= MCU_MOS_A_SW_PIN;
	GPIO_Init(MCU_MOS_A_SW_PORT, &stGpioInit);
	MCU_MOS_A_SW_SET(1);//init close mos, when current > 1A,open MOS, MCU->not gate-> and gate->mos driver

	stGpioInit.GPIO_Pin= LT7871_A_FAULT_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(LT7871_A_FAULT_PORT, &stGpioInit);		
}
/*
when power up or mcu reset, update 
*/
static void swpower_module_init(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(0, pLtObj);//LT7871_A_RUN_SET(0);
	//pLtObj->pfun->set_buck(1,pLtObj);//LT7871_A_BUCK_SET(1);
	//pLtObj->pfun->set_mode(0, pLtObj);//LT7871_A_MODE_SET(0);
	swpower_set_run_stop(pLtObj);
	swpower_set_buckmode(pLtObj);
	swpower_set_mode_discontinuous(pLtObj);
}

static void obj_init(LTC7871_OBJ *pLtObj,const LTC7871_FUN *pfun, LTC7871_REG * preg, u_char mod)
{
	pLtObj->pfun = pfun;
	pLtObj->preg = preg;
	pLtObj->mod = mod;
}

static void set_fan(u_char flag)		//		//1 on, 0 off
{
	if (flag)
	{
		MCU_FAN_SET(1);
	}
	else
	{
		MCU_FAN_SET(0);
	}
}

/*
swpower module   mcu->transistor od -> ltc7871 mode
flag 1 ->  ltc7871 mode = 0, continues
flag 0 -> float loating this pin results in burst mode in buck mode and
discontinuous mode in boost mode. 
*/
static void swpower_set_mode(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		LT7871_A_MODE_SET(1);  //continues continuous
		pLtObj->mode = LT7871_MODE_CONTINUOUS;
	}
	else
	{
		LT7871_A_MODE_SET(0);
		//pLtObj->mode = 0;
		pLtObj->mode = LT7871_MODE_DISCONTINUOUS;
	}
}

/*
mcu->ltc7871 run pin
flag 1 run, 0 stop
*/
static void swpower_set_run(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		LT7871_A_RUN_SET(1);//run
		//pLtObj->run = 1;
		pLtObj->run = LT7871_RUN_RUNNING;
	}
	else
	{
		LT7871_A_RUN_SET(0);//stop run
		//pLtObj->run = 0;
		pLtObj->run = LT7871_RUN_STOP;
	}
}
/*
swpoer module: mcu pin -> ltc7871 buck pin
*/
static void swpower_set_buck(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		LT7871_A_BUCK_SET(1);//BUCK
		//pLtBatteryObj->buck_boost = 0;
		//pLtObj->buck_boost = 1;
		pLtObj->buck_boost = LT7871_BUCK_MODE;
	}
	else
	{
		LT7871_A_RUN_SET(0); //boost
		//pLtBatteryObj->buck_boost = 1;
		//pLtObj->buck_boost = 0;
		pLtObj->buck_boost = LT7871_BOOST_MODE;
	}
}

//MCU->not gate-> and gate->mos driver
/*
flag 1 open mos
	 0 close mos
*/
static void swpower_set_mos_sw(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		MCU_MOS_A_SW_SET(0);
		//pLtObj->mos_sw = 1;
		pLtObj->mos_sw = LT7871_MOS_CLOSE;
	}
	else
	{
		MCU_MOS_A_SW_SET(1);
		//pLtObj->mos_sw = 0;
		pLtObj->mos_sw = LT7871_MOS_OPEN;
	}
}

static uint8_t swpower_read_fault(LTC7871_OBJ *pLtObj)
{
	u_char fault;
	
	fault = LT7871_A_FAULT_READ();
	pLtObj->fault = fault;
	
	return fault;
}
//end swpower module function

//battery module function


static void battery_pin_init(void)
{
	GPIO_InitTypeDef		stGpioInit;

	//init ltc7871_swpower pin init run, buck, mode, fault
	stGpioInit.GPIO_Pin= LT7871_B_RUN_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_OUT;
	stGpioInit.GPIO_OType = GPIO_OType_PP;
	stGpioInit.GPIO_PuPd = GPIO_PuPd_UP;
	stGpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LT7871_B_RUN_PORT, &stGpioInit);
	LT7871_B_RUN_SET(0);//stop run when init

	//buck pin share with swpower module
	//stGpioInit.GPIO_Pin= LT7871_A_BUCK_PIN;
	//GPIO_Init(LT7871_A_BUCK_PORT, &stGpioInit);
	//LT7871_A_BUCK_SET(1);//init sw power module in buck mode, 

	stGpioInit.GPIO_Pin= LT7871_B_MODE_PIN;
	GPIO_Init(LT7871_B_MODE_PORT, &stGpioInit);
	LT7871_B_MODE_SET(0);//set float mode pin

	//control mos pin
	stGpioInit.GPIO_Pin= MCU_MOS_B_SW_PIN;
	GPIO_Init(MCU_MOS_B_SW_PORT, &stGpioInit);
	MCU_MOS_B_SW_SET(1);//init close mos, when current > 1A,open MOS, MCU->not gate-> and gate->mos driver

	stGpioInit.GPIO_Pin= LT7871_B_FAULT_PIN;
	stGpioInit.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(LT7871_B_FAULT_PORT, &stGpioInit);
}

/*
battery module 
mcu->ltc7871 mode pin
1 -> mode = 0 force continuos
0 -> mode = 1 buck: Tying this pin to V5 enables discontinuous mode in buck or boost modes
*/
static void battery_set_mode(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		LT7871_B_MODE_SET(0);  //force continuos   connect gnd
		//pLtObj->mode = 1;
		pLtObj->mode = LT7871_MODE_CONTINUOUS;
	}
	else
	{
		LT7871_B_MODE_SET(1);
		//pLtObj->mode = 0;
		pLtObj->mode = LT7871_MODE_DISCONTINUOUS;
	}
	
}

/*
mcu->ltc7871 run pin
flag 1 run, 0 stop
*/
static void battery_set_run(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		LT7871_B_RUN_SET(1);//run
		//pLtObj->run = 1;
		pLtObj->run = LT7871_RUN_RUNNING;
	}
	else
	{
		LT7871_B_RUN_SET(0);//stop run
		//pLtObj->run = 0;
		pLtObj->run = LT7871_RUN_STOP;
	}
}

// mcu pin-> not gate->ltc7871 buck pin
/*
 flag 1 buck, 0 boost
*/
static void battery_set_buck(u_char flag,  LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		LT7871_A_BUCK_SET(0);//BUCK
		//pLtObj->buck_boost = 1;
		//pLtSwpowerObj->buck_boost = 0;
		pLtObj->buck_boost = LT7871_BUCK_MODE;
	}
	else
	{
		LT7871_A_RUN_SET(1);  //boost
		//pLtObj->buck_boost = 0;
		//pLtSwpowerObj->buck_boost = 1;
		pLtObj->buck_boost = LT7871_BOOST_MODE;
	}
}

//MCU->not gate-> and gate->mos driver
static void battery_set_mos_sw(u_char flag, LTC7871_OBJ *pLtObj)
{
	if (flag)
	{
		MCU_MOS_B_SW_SET(0);
		//pLtObj->mos_sw = 1;
		pLtObj->mos_sw = LT7871_MOS_CLOSE;
	}
	else
	{
		MCU_MOS_B_SW_SET(1);
		//pLtObj->mos_sw = 0;
		pLtObj->mos_sw = LT7871_MOS_OPEN;
	}
}

static uint8_t battery_read_fault(LTC7871_OBJ *pLtObj)
{
	u_char fault;
	fault = LT7871_B_FAULT_READ();
	//return LT7871_A_FAULT_READ();
	pLtObj->fault = fault;
	return fault;
}

//end battery function

static int swpower_set_buckmode(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_buck(1, pLtObj);
	pLtObj->pfun->set_buck(LT7871_BUCK_MODE, pLtObj);
	return 0;
}

static int swpower_set_boostmode(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_buck(0, pLtObj);
	pLtObj->pfun->set_buck(LT7871_BOOST_MODE, pLtObj);
	return 0;
}

static int swpower_set_mode_continuous(LTC7871_OBJ *pLtObj)
{
//	pLtObj->pfun->set_mode(1, pLtObj);
	pLtObj->pfun->set_mode(LT7871_MODE_CONTINUOUS, pLtObj);
	return 0;
}
static int swpower_set_mode_discontinuous(LTC7871_OBJ *pLtObj)
{
//	pLtObj->pfun->set_mode(0, pLtObj);
	pLtObj->pfun->set_mode(LT7871_MODE_DISCONTINUOUS, pLtObj);
	return 0;
}

static int swpower_set_mos_close(LTC7871_OBJ *pLtObj)
{
//	pLtObj->pfun->set_mos_sw(1, pLtObj);
	pLtObj->pfun->set_mos_sw(LT7871_MOS_CLOSE, pLtObj);
	return 0;
}

static int swpower_set_mos_open(LTC7871_OBJ *pLtObj)
{
	pLtObj->pfun->set_mos_sw(LT7871_MOS_OPEN, pLtObj);
	return 0;
}

static int swpower_set_run_running(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(1, pLtObj);
	pLtObj->pfun->set_run(LT7871_RUN_RUNNING, pLtObj);
	return 0;
}

static int swpower_set_run_stop(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(0, pLtObj);
	pLtObj->pfun->set_run(LT7871_RUN_STOP, pLtObj);
	return 0;	
}

static int swpower_set_current(int value)
{
	dac_ch1_set_voltage(value);
	return 0;
}

int lt7871_set_fan_run(LTC7871_OBJ *pLtObj)
{
	pLtObj->pfun->set_fan(1);
	return 0;
}

int lt7871_set_fan_stop(LTC7871_OBJ *pLtObj)
{
	pLtObj->pfun->set_fan(0);
	return 0;
}

static int battery_set_buckmode(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_buck(1, pLtObj);
	pLtObj->pfun->set_buck(LT7871_BUCK_MODE, pLtObj);
	return 0;
}

static int battery_set_boostmode(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_buck(0, pLtObj);
	pLtObj->pfun->set_buck(LT7871_BOOST_MODE, pLtObj);
	return 0;
}

static int battery_set_mode_continuous(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mode(1, pLtObj);
	pLtObj->pfun->set_mode(LT7871_MODE_CONTINUOUS, pLtObj);
	return 0;
}
static int battery_set_mode_discontinuous(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mode(0, pLtObj);
	pLtObj->pfun->set_mode(LT7871_MODE_DISCONTINUOUS, pLtObj);
	return 0;
}

static int battery_set_mos_close(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mos_sw(1, pLtObj);
	pLtObj->pfun->set_mos_sw(LT7871_MOS_CLOSE, pLtObj);
	return 0;
}

static int battery_set_mos_open(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mos_sw(0, pLtObj);
	pLtObj->pfun->set_mos_sw(LT7871_MOS_OPEN, pLtObj);
	return 0;
}

static int battery_set_run_running(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(1, pLtObj);
	pLtObj->pfun->set_run(LT7871_RUN_RUNNING, pLtObj);
	return 0;
}

static int battery_set_run_stop(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(0, pLtObj);
	pLtObj->pfun->set_run(LT7871_RUN_STOP, pLtObj);
	return 0;	
}

static int battery_set_current(int value)
{
	dac_ch2_set_voltage(value);
}


//x^8+x^2+^x+1  seed = 0x41
unsigned char  crc_8(unsigned char const *pstr,int length)
{
	unsigned char const* p= pstr;
	int len = length;
	unsigned char crc = 0x41;//ï¿½ï¿½ï¿½ï¿½ï¿½Ô¼ï¿½Êµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö²ï¿½ï¿½Ð³ï¿½Öµ
	
	unsigned char i = 0;
	while(len--)
	{
		crc ^= *p++;
		for(i = 8 ; i > 0 ; --i)
		{
			if(crc & 0x80)
			{								//ï¿½Ð¶ï¿½ï¿½ï¿½ï¿½Î»ï¿½Ç²ï¿½ï¿½ï¿½Îª1 
				crc =  (crc << 1)^ 0x07; 	// ï¿½ï¿½ï¿½Î»Îª1ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Òªï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ò»Î»ï¿½ï¿½È»ï¿½ï¿½ï¿½ï¿½0x31ï¿½ï¿½ï¿½ 
			}else
			{
				crc = (crc << 1); 
			} 
		} 
	} 
	return crc;
}


//static 


//x^8+x^2+^x+1  seed = 0x41
static u_char lt7871_pec_crc(unsigned char const *pstr,int length)
{
	return crc_8(pstr, length);
}

void lt7871_write_buffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );
   	SPI_NSS_SET(0);
   	delay_us(1);

 	SPI_CLK_SET(1);
	delay_us(1);   	//DWT_Delay(2);
  	
    spi_write( addr & 0xFE );	//bit0: 0 write , 1 read
    for( i = 0; i < size; i++ )
    {
        spi_write( buffer[i] );
    }

   //NSS = 1;
   //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
	SPI_CLK_SET(1); 
	delay_us(1);
	SPI_NSS_SET(1);
	delay_us(1);
}


void lt7871_read_buffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

   	SPI_NSS_SET(0);
   	delay_us(1);
   	
	SPI_CLK_SET(1);
	delay_us(1);   	//DWT_Delay(2);
	
    spi_write((addr|0x01));
	XPRINTF((11, "ADDR=%02x addr-rw=%02x\r\n", addr, ( addr | 0x01)));
    for( i = 0; i < size; i++ )
    {
        buffer[i] = spi_read( );//bit0: 0 write , 1 read
    }
     MEM_DUMP(11, "LTRD", buffer, 2);
    //NSS = 1;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
    SPI_CLK_SET(1); 
    delay_us(1);
    SPI_NSS_SET(1);
    delay_us(1);
}

//mcu pin -> not gate -> battery ltc7871 css pin
void lt7871_write_buffer_battery( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );
   	SPI_NSS_SET(1); //select chip
   	delay_us(1);

 	SPI_CLK_SET(1);
	delay_us(1);   	//DWT_Delay(2);
  	
    spi_write( addr & 0xFE );	//bit0: 0 write , 1 read
    for( i = 0; i < size; i++ )
    {
        spi_write( buffer[i] );
    }

   //NSS = 1;
   //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
	SPI_CLK_SET(1); 
	delay_us(1);
	SPI_NSS_SET(0);
	delay_us(1);
}

//mcu pin -> not gate -> battery ltc7871 css pin
void lt7871_read_buffer_battery( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

   	SPI_NSS_SET(1);
   	delay_us(1);
   	
	SPI_CLK_SET(1);
	delay_us(1);   	//DWT_Delay(2);
	
    spi_write((addr|0x01));
	XPRINTF((11, "ADDR=%02x addr-rw=%02x\r\n", addr, ( addr | 0x01)));
    for( i = 0; i < size; i++ )
    {
        buffer[i] = spi_read( );//bit0: 0 write , 1 read
    }
     MEM_DUMP(11, "LTRD", buffer, 2);
    //NSS = 1;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
    SPI_CLK_SET(1); 
    delay_us(1);
    SPI_NSS_SET(0);
    delay_us(1);
}



void lt7871_write( uint8_t addr, uint8_t data )
{
    lt7871_write_buffer( addr, &data, 1 );
}

void lt7871_read( uint8_t addr, uint8_t *data )
{
    lt7871_read_buffer( addr, data, 1 );
}




//read ï¿½Ä´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ð£ï¿½ï¿½ï¿½ï¿½
bool lt7871_read_reg(uint8_t addr, uint8_t *data, LTC7871_OBJ *pLtObj)
{
	bool flag = false;
	u_char buf[3] = {0x00};
	int i = 0;
	u_char pec = 0x00;

	buf[0] = addr|0x01;
	//lt7871_set_reg_bit
	while(i < 5)
	{
		i++;
		pLtObj->pfun->lt7871_read_buffer(addr,(uint8_t *)&buf[1], 2);//lt7871_read_buffer(addr,(uint8_t *)&buf[1], 2);
		pec = crc_8((unsigned char const*)buf, 2);
		if (pec == buf[2])
		{
			flag = true;
			data[0] = buf[1];
			XPRINTF((11, "check ok PEC=%02x buf[1]=%02x\r\n", pec, buf[2]));
			MEM_DUMP(11, "REG-", buf, 3);
			break;
		}
	}
	return flag;
}

bool lt7871_write_reg(uint8_t addr, uint8_t data, LTC7871_OBJ *pLtObj)
{
	u_char buf[3]={0x00};
	u_char bufr[3] = {0x00};
	u_char pec = 0x00;
	int i = 0;
	bool flag = false;
	
	buf[0] = addr&0xFE;//
	buf[1] = data;
	buf[2] = crc_8((unsigned char const*)buf, 2);

	bufr[0] = addr|0x01;
	//lt7871_clear_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj);
	while(i < 5)
	{
		i++;
		pLtObj->pfun->lt7871_write_buffer(addr, (uint8_t*)&buf[1], 2);//lt7871_write_buffer(addr, (uint8_t *)&buf[1], 2);
		pLtObj->pfun->lt7871_read_buffer(addr, (uint8_t*)&bufr[1],2);//lt7871_read_buffer(addr,(uint8_t *)&bufr[1], 2);
		pec = crc_8((unsigned char  const *)bufr, 2);
		if (pec == bufr[2] && data == bufr[1])
		//if (pec == bufr[2]) //not compare write data and read data, because when write MFR_CHIP_RESET,read data not equal write data
		{
			flag = true;
			MEM_DUMP(11, "wreg", buf, 3);
			MEM_DUMP(11, "rreg", bufr, 3);			
			break;
		}
	}
	//lt7871_set_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj);
	return flag;
}

//¸ù¾ÝµçÑ¹£¬¼ÆËãVlowµçÁ÷µÄÊä³ö£¬Ð´Èë¼Ä´æÆ÷
/*
static u_char cal_buck_current(int v)
{
	float current;
	u_char value=0x00;
	current =(1.2*(1+(float)(RB/RA))-(float)v)/RB*1000;
	//printfFloat("¼ÆËãÊýÖµ",v); 
	//printfFloat("¼ÆËãµçÁ÷",current);

	if(current<0)
	{
		if(current<-64.5f)
		{
			current=-64.0f;
		}
		else
		{
			current=current-0.5f;
		}
		current=-current;
		value = 0x80-(u_char)(current);
	}
	else
	{
		value=(u_char)current;
	}

	//printfFloat("¼ÆËãµçÁ÷1",current);
	XPRINTF((10, "(int)current:%d\r\n",(int)current);
	XPRINTF((10, "(u_char)(current)%x\r\n",(u_char)(current)));
	XPRINTF((10, "value:%x\r\n",value));
	return value;   
} 
*/

//¸ù¾ÝµçÑ¹£¬¼ÆËãVlowµçÁ÷µÄÊä³ö£¬Ð´Èë¼Ä´æÆ÷
//1uA ->  1uA *39K = 0.39V->0.39V *100 39
//vlow   xxx->  xx.x
//current =(1.2*(1+(float)(RB/RA))-(float)v)/RB*1000;
/*
	1.2->1.2*100
	40->1+(RB/RA) = 1+390/10 = 40
*/
//->(120*40)-v*10)*100 /39  ->xxx.x  
//setv  [40.0V-60.0V]->XX.X->[400-600]
static u_char buck_current(int setv)
{
	int current;
	u_char value=0x00;
	current = (LT7871_DEFAULT_LOW_OUT-setv*10)/39;//À©Õ¹100£¬±ãÓÚ4Éá5Èë
	int index = 0;
	if(current<0)
	{
		if(current<-64)
		{
			current = -64;
		}
		else
		{
			index = 0 - current;
		}
		value = _sink_current[index];
		XPRINTF((6, "N  index = %d, setv = %d, value = %02x\r\n", index, setv, value));
	}
	else
	{
		if (current > 63)
		{
			index = 63;
		}
		else
		{
			index = current;
		}
		value = _source_current[index];
		XPRINTF((6, "P  index = %d, setv = %d, value = %02x\r\n", index, setv, value));
		
	}
	//XPRINTF((10, ""));
	return value;   
} 
/*

VHIGH = 1.2V(1+RD/RC) - IDAC*RD = 48.35V
RD = 499K RC = 12.7K
1uA ->  IDAC*499K = 1uA*499k=0.499 -> 50
*/

static u_char boost_current(int setv)
{
	int current;
	u_char value=0x00;
	current = (LT7871_DEFAULT_HIGH_OUT-setv*10)/50;//À©Õ¹100£¬±ãÓÚ4Éá5Èë
	int index = 0;
	if(current<0)
	{
		if(current<-64)
		{
			current = -64;
		}
		else
		{
			index = 0 - current;
		}
		value = _sink_current[index];
		XPRINTF((10, "N  index = %d, setv = %d, value = %02x\r\n", index, setv, value));
	}
	else
	{
		if (current > 63)
		{
			index = 63;
		}
		else
		{
			index = current;
		}
		value = _source_current[index];
		XPRINTF((10, "P  index = %d, setv = %d, value = %02x\r\n", index, setv, value));
		
	}
	//XPRINTF((10, ""));
	return value;   
} 

int lt7871_set_buckmode(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_buck(1, pLtObj);
	pLtObj->pfun->set_buck(LT7871_BUCK_MODE, pLtObj);
	return 0;
}

int lt7871_set_boostmode(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_buck(0, pLtObj);
	pLtObj->pfun->set_buck(LT7871_BOOST_MODE, pLtObj);
	return 0;
}

int lt7871_set_mode_continuous(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mode(1, pLtObj);
	pLtObj->pfun->set_mode(LT7871_MODE_CONTINUOUS, pLtObj);
	return 0;
}
int lt7871_set_mode_discontinuous(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mode(0, pLtObj);
	pLtObj->pfun->set_mode(LT7871_MODE_DISCONTINUOUS, pLtObj);
	return 0;
}

int lt7871_set_mos_close(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mos_sw(1, pLtObj);
	pLtObj->pfun->set_mos_sw(LT7871_MOS_CLOSE, pLtObj);
	return 0;
}

int lt7871_set_mos_open(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_mos_sw(0, pLtObj);
	pLtObj->pfun->set_mos_sw(LT7871_MOS_OPEN, pLtObj);
	return 0;
}

int lt7871_set_run_running(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(1, pLtObj);
	pLtObj->pfun->set_run(LT7871_RUN_RUNNING, pLtObj);
	return 0;
}

int lt7871_set_run_stop(LTC7871_OBJ *pLtObj)
{
	//pLtObj->pfun->set_run(0, pLtObj);
	pLtObj->pfun->set_run(LT7871_RUN_STOP, pLtObj);
	return 0;	
}

uint8_t lt7871_set_reg_bit(uint8_t addr, uint8_t bit, LTC7871_OBJ *pLtObj)
{
	u_char bdata;
	bool flag = false;

	flag = lt7871_read_reg(addr, &bdata, pLtObj);
	if (flag)
	{
		//bdata &= (~bit);
		if ((bdata & bit) == bit)
		{
			flag = true;
		}
		else
		{
			bdata |= bit;
			XPRINTF((11, "lt7871_set_reg_bit bdata = %02x, bit = %02x\r\n", bdata,bit));
			flag = lt7871_write_reg(addr, bdata, pLtObj);
		}
	}
	
	return flag;
}

uint8_t lt7871_clear_reg_bit(uint8_t addr, uint8_t bit, LTC7871_OBJ *pLtObj)
{
	u_char bdata;
	bool flag = false;

	flag = lt7871_read_reg(addr, &bdata, pLtObj);
	if (flag)
	{
		flag = false;
		if ((bdata & bit) == bit)
		{
			bdata &= (~bit);
			flag = lt7871_write_reg(addr, bdata, pLtObj);
			XPRINTF((11, "lt7871_clear_reg_bit bdata = %02x, bit = %02x\r\n", bdata,bit));
		}
	}
	
	return flag;	
}



//Vlow = 1.2V (1+RB/RA)-IDAC*RB
/*
return -1 failed
       0  successful
*/
int lt7871_set_buck_voltage(int voltage, LTC7871_OBJ *pLtObj)
{
	//int nResult = -1;
	u_char current = 0;
	
	//[39.0V-59.0v]
	if ((voltage < LT7871_OUT_V_MIN) || (voltage > LT7871_OUT_V_MAX))
	{
		return -1;
	}
	current = buck_current(voltage);
	lt7871_clear_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj);
	lt7871_write_reg(MFR_IDAC_VLOW, current, pLtObj);
	lt7871_set_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj);
	return 0;
}


int lt7871_set_boost_voltage(int voltage, LTC7871_OBJ *pLtObj)
{
	u_char current = 0;
	if ((voltage < LT7871_OUT_V_MIN) || (voltage > LT7871_OUT_V_MAX))
	{
		return -1;
	}
	current = boost_current(voltage);
	lt7871_clear_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj); //enable write
	lt7871_write_reg(MFR_IDAC_VHIGH, current, pLtObj);
	lt7871_set_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj); //disable write
    return 0;
}


int lt7871_set_mfr_idac_setcurrent(u_char current, LTC7871_OBJ *pLtObj)
{
	lt7871_clear_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj); //enable write
	lt7871_write_reg(MFR_IDAC_SETCUR, current, pLtObj);
	lt7871_set_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj); //disable write
    return 0;
}

int lt7871_set_mfr_idac_setcurrent_zero( LTC7871_OBJ *pLtObj)
{
	lt7871_clear_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj); //enable write
	lt7871_write_reg(MFR_IDAC_SETCUR, 0x10, pLtObj);
	lt7871_set_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP, pLtObj); //disable write
    return 0;
}

/*
k = 40, ILIM voltage 0v or 1/4 V5
k = 20, ILIM voltage 3/4 v5 or v5
RENSE = 0.002 R
Iall total average inductor current
Vsetcur = k*Ilmax *Rsense/6
current  format xxx.x
*/
int lt7871_set_current(int current, LTC7871_OBJ *pLtObj)
{
	u_char idac_setcurrent = 0;
	u_short voltage = 0;

	/*
	when mfr_idac_setcur value is not MFR_IDAC_SETCUR_0UA, set value to MFR_IDAC_SETCUR_0UA
	for dac out value to control current
	*/
	lt7871_read_reg(MFR_IDAC_SETCUR, &idac_setcurrent,pLtObj);
	if (idac_setcurrent != MFR_IDAC_SETCUR_0UA)
	{
		lt7871_set_mfr_idac_setcurrent_zero(pLtObj);
	}
	//current xxxx->xxx.x
	//current *10, need to /10
	//Rsense *1000  
	//voltage uint is mV[0-1250]
	voltage = (LT7871_SETCUR_K*current*LT7871_SETCUR_RSENSE)/(10*6);

	if (voltage > LT7871_SETCURV_MAX)
	{
		voltage = (LT7871_SETCURV_MAX-LT7871_SETCURV_ERROR); //for dac error
	}

	pLtObj->pfun->set_current(voltage);
	return 0;
}

static bool is_swpower_to_battery(LTC7871_OBJ *pLtSobj, LTC7871_OBJ *pLtBobj)
{
	bool flag = false;
//	if (pLtSobj->)	
	return flag;
}

static bool is_battery_to_swpower(LTC7871_OBJ *pLtSobj, LTC7871_OBJ *pLtBobj)
{
	bool flag = false;
	if ((pLtSobj->run == LT7871_RUN_RUNNING) && (pLtBobj->run == LT7871_RUN_RUNNING))
	{
		
	}
	
	return flag;
}

/*
swpower buck mode const v, 
battery module boost mode, battery const voltage, battery const voltage is used to battery cosnt voltage charge
param:
power_voltage 		set swpower module buck mode, const buck voltage
battery_voltage 	set battery module boost mode, const boost voltage
pLtSobj				swopwer module obj
pLtBobj				battery module obj

note:this function is used to charge battery const voltage
*/
int lt7871_swpower_v_to_battery_v(int power_voltage, int battery_voltage, LTC7871_OBJ *pLtSobj, LTC7871_OBJ *pLtBobj)
{
	
	return 0;
}
/*
swpower buck mode const v,
battery module boost mode, battery const current, battery const current is used to battery const current charge 

param:
power_voltage 		set swpower module buck mode, const buck voltage
battery_current 	set battery module boost mode, const boost current
pLtSobj				swopwer module obj
pLtBobj				battery module obj

note:this function is used to charge battery const current
*/
int lt7871_swpower_v_to_battery_c(int power_voltage, int battery_current, LTC7871_OBJ *pLtSobj, LTC7871_OBJ *pLtBobj)
{
	
	return 0;
}
/*
param:
power_voltage 		set swpower module boost mode, const boost voltage
battery_voltage 	set battery module buck mode, const buck voltage
pLtSobj				swopwer module obj
pLtBobj				battery module obj

note:this function is used to discharge battery, spower port discharge const voltage
*/
int lt7871_battery_v_to_swpower_v(int power_voltage, int battery_voltage, LTC7871_OBJ *pLtSobj, LTC7871_OBJ *pLtBobj)
{
	
	return 0;
}

/*
param:
power_voltage 		set swpower module boost mode, const boost current
battery_current 	set battery module buck mode, const buck voltage
pLtSobj				swopwer module obj
pLtBobj				battery module obj

note:this function is used to discharge battery , swpower port discharge const current
*/
int lt7871_battery_v_to_swpower_c(int power_current, int battery_voltage, LTC7871_OBJ *pLtSobj, LTC7871_OBJ *pLtBobj)
{
	
	return 0;
}



PROCESS_THREAD(lt7871, ev, data)
{
	static struct etimer _et;
	static LTC7871_OBJ *pLtObj;
    static LTC7871_OBJ *pLtBobj;//battery module obj
	u_char ltdata;
	static u_char bd[2] = {0x00};
	u_char idacC = 0;
	PROCESS_BEGIN();
	pLtObj = &tgLtc7871Obj_Swpower;
    pLtBobj = &tgLtc7871Obj_Battery;
	spi_cfg( );
	obj_init(pLtObj, &tgLtc7871_fun_swpower, &tgLtc7871Reg_Swpower, 1); //first function need to be run
	pLtObj->pfun->pin_init( );
	swpower_module_init(pLtObj);
	swpower_set_mode_continuous(pLtObj);
	swpower_set_buckmode(pLtObj);//pLtObj->pfun->set_buck(1, pLtObj);
	swpower_set_mos_close(pLtObj);//pLtObj->pfun->set_mos_sw(1, pLtObj);//open mos
	swpower_set_run_running(pLtObj);//pLtObj->pfun->set_run(1, pLtObj);

	dac_cfg( );//
		//DAC_Init(DAC_TypeDef * DACx, uint32_t DAC_Channel, DAC_InitTypeDef * DAC_InitStruct)
	etimer_set(&_et, 1000);
	PROCESS_WAIT_UNTIL(etimer_expired(&_et));
	lt7871_set_mfr_idac_setcurrent_zero(pLtObj);
	//initCtrl( );
	while(1)
	{
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));

	
		//XPRINTF((11,"start1 read------------------------reg\r\n"));
		//lt7871_read_reg(MFR_CHIP_CTRL, bd, pLtObj);
		//xprintf("lt7871rea\r\n");
		//XPRINTF((11,"bd = %02x\r\n", bd[0]));
		//XPRINTF((11,"end1 read------------------------reg\r\n"));
		
		//XPRINTF((11,"start1 write------------------------reg\r\n"));
		//ltdata = 0x01;
		//lt7871_write_reg(MFR_CHIP_CTRL, ltdata, pLtObj);
		//lt7871_set_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_RESET,  pLtObj);
		//lt7871_read_reg(MFR_CHIP_CTRL, bd, pLtObj);
		//XPRINTF((10,"bd  MFR_CHIP_CTRL MFR_CHIP_WP_INHIBITED = %02x\r\n", bd[0]));
		//XPRINTF((11,"end1 write------------------------reg\r\n"));
		
		//XPRINTF((11,"start2 write------------------------reg\r\n"));
		//lt7871_clear_reg_bit(MFR_CHIP_CTRL, MFR_CHIP_WP,  pLtObj);
		//lt7871_read_reg(MFR_CHIP_CTRL, bd, pLtObj);
		//XPRINTF((10,"bd  MFR_CHIP_CTRL MFR_CHIP_WP_ALLOWED = %02x\r\n", bd[0]));
		
		//ltdata = 0x00;
		//lt7871_write_reg(MFR_CHIP_CTRL, ltdata, pLtObj);
		//XPRINTF((11,"end2 write------------------------reg\r\n"));	

		/*
		idacC = buck_current(440);
		lt7871_write_reg(MFR_IDAC_VLOW, idacC, pLtObj);
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));
		*/
		//idacC = buck_current(380);
		//lt7871_write_reg(MFR_IDAC_VLOW, idacC, pLtObj);		//buck_current(540);
		//etimer_set(&_et, 10000);
		//PROCESS_WAIT_UNTIL(etimer_expired(&_et));

		/*
		idacC = buck_current(420);
		lt7871_write_reg(MFR_IDAC_VLOW, idacC, pLtObj);		//buck_current(540);
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));	

		idacC = buck_current(390);
		lt7871_write_reg(MFR_IDAC_VLOW, idacC, pLtObj);		//buck_current(540);
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));		
		*/
		//buck_current(580);

		lt7871_set_buck_voltage(440, pLtObj);
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));

		lt7871_set_buck_voltage(430, pLtObj);
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));

		lt7871_set_buck_voltage(415, pLtObj);
		etimer_set(&_et, 10000);
		PROCESS_WAIT_UNTIL(etimer_expired(&_et));
		
	}
	PROCESS_END();
}


void lt7871_start(void)
{
	process_start(&lt7871, NULL);
}


