#ifndef _LTC_7871_H
#define _LTC_7871_H

/*check define and low out limit define , high out limit define*/
#define LT7871_DEFAULT_LOW_OUT		4720	//this value default 1.2*(1+RB/RA)->RB=390K, RA=10K, default = 48.0V *100 for clacmulate current MFR_IDAC_VLOW
#define LT7871_DEFAULT_HIGH_OUT		4835	//
#define LT7871_OUT_V_MIN			390		//39.0v
#define LT7871_OUT_V_MAX			590 	//59.0v
#define LT7871_SETCURV_MAX			1250	//1.25V
#define LT7871_SETCUR_K				20
#define LT7871_SETCUR_RSENSE		2		//0.002
#define LT7871_SETCURV_ERROR		50


//state define
#define LT7871_BUCK_MODE			1
#define LT7871_BOOST_MODE			0
#define LT7871_MODE_CONTINUOUS		1		//continuous
#define LT7871_MODE_DISCONTINUOUS	0
#define LT7871_RUN_RUNNING			1
#define LT7871_RUN_STOP				0
#define LT7871_MOS_CLOSE			1
#define LT7871_MOS_OPEN				0

#define RA    10000			//r 10k
#define RB    390000		//k 390k
#define RC    12700			//12.7k
#define RD    499000		//499k

//regaddr
#define MFR_FAULT   		(0x01<<1)		//R
#define MFR_OC_FAULT		(0x02<<1)		//R
#define MFR_NOC_FAULT		(0x03<<1)		//R
#define MFR_STATUS			(0x04<<1)		//R
#define MFR_CONFIG1			(0x05<<1)		//R
#define MFR_CONFIG2			(0x06<<1)		//R
#define MFR_CHIP_CTRL		(0x07<<1)		//R/W  	0x00
#define MFR_IDAC_VLOW		(0x08<<1)		//R/W	0x00
#define MFR_IDAC_VHIGH		(0x09<<1)		//R/W	0x00
#define MFR_IDAC_SETCUR		(0x0a<<1)		//R/W	0x00
#define MFR_SSFM			(0x0b<<1)		//R/W	0x00

//MFR_FAULT reg 0x01 read
#define MFR_FAULT_VLOW_0V	(1<<6)		//The OVLOW pin is higher than 1.2V threshold
#define MFR_FAULT_VHIGH_0V	(1<<5)		//The OVHIGH pin is higher than 1.2V threshold.
#define MFR_FAULT_VHIGH_UV	(1<<4)		//The UVHIGH pin is less than 1.2V threshold.
#define MFR_FAULT_DRVCC_UV	(1<<3)		//The DRVCC pin is undervoltage
#define MFR_FAULT_V5_UV		(1<<2)		//The V5 pin is undervoltage
#define MFR_FAULT_VREF_BAD	(1<<1)		//The internal reference self-check fails.
#define MFR_FAULT_OVER_TEMP	(1<<0)		//An over temperature fault has occurred.

//MFR_OC_FAULT reg 0x02 read
#define MFR_OC_FAULT_OC_FAULT6 (1<<5)	//Channel 6 overcurrent fault has occurred.
#define MFR_OC_FAULT_OC_FAULT5 (1<<4)	//Channel 5 overcurrent fault has occurred.
#define MFR_OC_FAULT_OC_FAULT4 (1<<3)	//Channel 4 overcurrent fault has occurred.
#define MFR_OC_FAULT_OC_FAULT3 (1<<2)	//Channel 3 overcurrent fault has occurred.
#define MFR_OC_FAULT_OC_FAULT2 (1<<1)	//Channel 2 overcurrent fault has occurred.
#define MFR_OC_FAULT_OC_FAULT1 (1<<0)	//Channel 1 overcurrent fault has occurred.


//MFR_NOC_FAULT 0x03 read
#define MFR_NOC_FAULT_NOC_FAULT_6	(1<<5) //Channel 6 negative overcurrent fault has occurred
#define MFR_NOC_FAULT_NOC_FAULT_5	(1<<4) //Channel 5 negative overcurrent fault has occurred
#define MFR_NOC_FAULT_NOC_FAULT_4	(1<<3) //Channel 4 negative overcurrent fault has occurred
#define MFR_NOC_FAULT_NOC_FAULT_3	(1<<2) //Channel 3 negative overcurrent fault has occurred
#define MFR_NOC_FAULT_NOC_FAULT_2	(1<<1) //Channel 2 negative overcurrent fault has occurred
#define MFR_NOC_FAULT_NOC_FAULT_1	(1<<0) //Channel 6 negative overcurrent fault has occurred

//MFR_STATUS 0x04 read
#define MFR_STATUS_SS_DONE		(1<<2)	//The soft-start is finished
#define MFR_STATUS_MAX_CURRENT	(1<<1)	//The maximum current programmed by the ILIM pin is reached
#define MFR_STATUS_PGOOD		(1<<0)	//The regulated VLOW/VHIGH is within ¡À10% regulation windows.

//MFR_CONFIG1	0x05 read
#define MFR_CONFIG1_SERCUR_WARNING (1<<5)	//The SETCUR pin is programmed to be above 1.25V
#define MFR_CONFIG1_DRVCC_SET_00_5V (0<<3)	//The DRVCC is programmed to 5V.	
#define MFR_CONFIT1_DRVCC_SET_01_8V	(1<<3)	//The DRVCC is programmed to 8V.
#define MFR_CONFIG1_DRVCC_SET_10_10V (2<<3)	//The DRVCC is programmed to 10V
#define MFR_CONFIG1_ILIM_SET_000_10MV	(0) //he maximum current sense threshold is programmed to 10mV
#define MFR_CONFIG1_ILIM_SET_001_20MV	(1) //he maximum current sense threshold is programmed to 20mV
#define MFR_CONFIG1_ILIM_SET_010_30MV	(2) //he maximum current sense threshold is programmed to 30mV
#define MFR_CONFIG1_ILIM_SET_011_40MV	(3) //he maximum current sense threshold is programmed to 40mV
#define MFR_CONFIG1_ILIM_SET_100_50MV	(4) //he maximum current sense threshold is programmed to 50mV

//MFR_CONFIG2 0x06 read
#define MFR_CONFIG2_BUST	(1<<4)		//The controller is in burst mode operation.
#define MFR_CONFIG2_DCM		(1<<3)		//The controller is in DCM
#define MFR_CONFIG2_HIZ		(1<<2)		//The controller is in Hi-Z mode
#define MFR_CONFIG2_SPRD	(1<<1)		//The controller is in spread spectrum mode.
#define MFR_CONFIG2_BUCKBOOST	(1<<0)			//The controller is in boost mode
//#define MFR_CONFIG2_BUCK	(1)			//The controller is in buck mode.


//MFR_CHIP_CTRL  0x07 rw 
#define MFR_CHIP_CTRL_CML		(1<<2)	//A communication fault related to PEC during writing registers has occurred. Write 1 to this bit will clear the CML.
#define MFR_CHIP_RESET			(1<<1)	//Sticky bit, reset all R/W registers.
#define MFR_CHIP_WP				(1<<0)	//write allowed for all three IDAC registers, and MFR_SSFM register.
//#define MFR_CHIP_WP_INHIBITED	(1)		//Write inhibited for all three IDAC registers, and MFR_SSFM register.


/*
The MFR_IDAC_VLOW stores the current DAC value to program the VLOW voltage by injecting the current DAC output
to the VFBLOW pin. It is formatted as a 7-bit two¡¯s complement value. Setting BIT[6] = 0 means sourcing current from
the VFBLOW pin; and BIT[6] = 1 means sinking current. The detail is listed in Table 8. The DAC current is only injected
to the VFBLOW pin in buck mode. Sinking current will cause VLOW to rise. The default value for this register is 0x00.
Writes to this register are inhibited when the WP, BIT[0] in MFR_CHIP_CTRL, is set high.
*/
//MFR_IADC_VLOW	0x08 rw
#define MFR_IDAC_VLOW_N64UA	(1<<6)
#define MFR_IDAC_VLOW_32UA	(1<<5)
#define MFR_IDAC_VLOW_16UA	(1<<4)
#define MFR_IDAC_VLOW_8UA	(1<<3)
#define MFR_IDAC_VLOW_4UA	(1<<2)
#define MFR_IDAC_VLOW_2UA	(1<<1)
#define MFR_IDAC_VLOW_1UA	(1<<0)

/*
The MFR_IDAC_VHIGH stores the current DAC value to program the VHIGH voltage by injecting the current DAC output
to the VFBHIGH pin. It is formatted as a 7-bit two¡¯s complement value. Setting BIT[6] = 0 means sourcing current from
the VFBHIGH pin; and BIT[6] = 1 means sinking current. The detail is listed in Table 8. The DAC current is only injected
to the VFBHIGH pin in boost mode. Sinking current will cause VHIGH to rise in boost mode. The default value for this
register is 0x00. Writes to this register are inhibited when the WP, BIT[0] in MFR_CHIP_CTRL, is set high.
*/
//MFR_IADC_VHIGH 0x09 rw
#define MFR_IDAC_VHGIH_N64UA	(1<<6)
#define MFR_IDAC_VHGIH_32UA	(1<<5)
#define MFR_IDAC_VHGIH_16UA	(1<<4)
#define MFR_IDAC_VHGIH_8UA	(1<<3)
#define MFR_IDAC_VHGIH_4UA	(1<<2)
#define MFR_IDAC_VHGIH_2UA	(1<<1)
#define MFR_IDAC_VHGIH_1UA	(1<<0)

/*
The MFR_IDAC_SETCUR stores the current DAC value to program the sourcing current of the SETCUR pin.
It is formatted as a 5-bit two¡¯s complement value. The default value for this register is 0x00 and the SETCUR pin originally
sources 16¦ÌA. This register can program the SETCUR pin sourcing current from 0 to 31¦ÌA as shown in the Table 9.
Writes to this register are inhibited when the WP, BIT[0] in MFR_CHIP_CTRL, is set high.
*/
//MFR_IDAC_SETCUR
#define MFR_IDAC_SETCUR_16UA		(0<<4)
#define MFR_IDAC_SETCUR_8UA			(1<<3)
#define MFR_IDAC_SETCUR_4UA			(1<<2)
#define MFR_IDAC_SETCUR_2UA			(1<<1)
#define MFR_IDAC_SETCUR_1UA			(1<<0)
#define MFR_IDAC_SETCUR_0UA			(1<<4)


//MFR_SSFM
//FSR  ->Frequency Spread Range
#define MFR_SSFM_FSR_12		(0<<3)	//+-12%
#define MFR_SSFM_FSR_15		(1<<3)	//+-15%
#define MFR_SSFM_FSR_10		(2<<3)	//+-10%
#define MFR_SSFM_FSR_8		(3<<3)  //+-8%
//MSF Modulation Signal Frequency
#define MFR_SSFM_MSF_512	(0)		//Controller Switching Frequency/512
#define MFR_SSFM_MSF_1024	(1)		//Controller Switching Frequency/1024
#define MFR_SSFM_MSF_2048	(2)		//Controller Switching Frequency/2048
#define MFR_SSFM_MSF_4096	(3)		//Controller Switching Frequency/4096
#define MFR_SSFM_MSF_256	(4)		//Controller Switching Frequency/256
#define MFR_SSFM_MSF_128	(5)		//Controller Switching Frequency/128
#define MFR_SSFM_MSF_64		(6)		//Controller Switching Frequency/64
//#define MFR_SSFM_MSF_512	(7)		//Controller Switching Frequency/512


typedef struct {
	u_char addr;
	u_char value;
}LT7871_REG;

//typedef int(*uart_handler_t)(unsigned char);
//typedef u_char(*reg_read)(LT7871_REG *preg);

typedef struct _ltc7871_reg LTC7871_REG;
typedef struct _obj_ltc7871 LTC7871_OBJ;
typedef struct _ltc7871_fun LTC7871_FUN;

struct _ltc7871_fun{
	void (*pin_init)(void);		//init control pin, first run
	void (*lt7871_obj_init)(LTC7871_OBJ *pLtObj, const LTC7871_FUN *pfun, LTC7871_REG * preg, u_char mod);
	void (*lt7871_write_buffer)( uint8_t addr, uint8_t *buffer, uint8_t size );
	void (*lt7871_read_buffer)( uint8_t addr, uint8_t *buffer, uint8_t size );
	void (*set_fan)(u_char flag);							//1 on, 0 off
	void (*set_mode)(u_char flag, LTC7871_OBJ *pLtObj);		//1 Tying this pin to SGN,Denables forced continuous mode in buck or boost modes; 0 
															//Floating this pin results in burst mode in buck mode and discontinuous mode in boost mode
	void (*set_run)(u_char flag, LTC7871_OBJ *pLtObj);		//1 run , 0 stop
	//1 buck mode, vhigh->vlow; 0 boost mode, blow->vhigh
	void (*set_buck)(u_char flag,LTC7871_OBJ *pLtObj);		
	void (*set_mos_sw)(u_char flag, LTC7871_OBJ *pLtObj);	//1 open mos, 0 close mos
	uint8_t (*read_fault)( LTC7871_OBJ *pLtObj);		//read fault pin
	int (*set_current)(int value);
};

struct _ltc7871_reg{
	/*
	//addr:0x01 
	he MFR_FAULT returns a one-byte summary of the most critical faults
	*/
	LT7871_REG fault;	 
	LT7871_REG oc_fault;
	LT7871_REG noc_fault;
	LT7871_REG status;
	LT7871_REG config1;
	LT7871_REG config2;
	LT7871_REG chip_ctrl;
	LT7871_REG idac_vlow;
	LT7871_REG idac_vhigh;
	LT7871_REG idac_setcur;
	LT7871_REG ssfm;
	//u_char (*read)(u_char addr, u_char data, *preg);
	//u_char (*write)(LT7871_REG *preg);
};


/*
	void (*set_fan)(u_char flag);		//1 on, 0 off
	void (*set_mode)(u_char flag);		//1 Tying this pin to SGN,Denables forced continuous mode in buck or boost modes; 0 
										//Floating this pin results in burst mode in buck mode and discontinuous mode in boost mode
	void (*set_run)(u_char flag);		//1 run , 0 stop
	void (*set_buck)(u_char flag);		//1 buck mode, vhigh->vlow; 0 boost mode, blow->vhigh
	void (*set_mos_sw)(u_char flag);	//1 open mos, 0 close mos
	uint8_t (*read_fault)(void);		//read fault pin
*/
struct _obj_ltc7871{
	const LTC7871_FUN *pfun;
	LTC7871_REG *preg;
	u_char mod;			//1 swpower module, 2 battery module.
	u_char buck_boost;	//1 buck, 0 boost;
	u_char run;			//1 run , 0 stop
	u_char mode;		//0 float buck:burst mode, boost:discontinuous;1 buck:force continuous, boost:force continuous
	u_char mos_sw;		//0 close, 1 open
	u_char fault;		//ltc7871 fault state
};	

#endif



