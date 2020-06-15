#ifndef _DEV_INFO_H
#define _DEV_INFO_H

#include "bpmsp.h"
#include "rn8209d.h"
#include "arch_encrypt.h"

#define NODE_SOFTWARE_V				0x10  //v1.0
#define NODE_HARDWARE_V				0x10  //v1.0
#define NODE_MADE_INFO				"JY"


#define MAX_SAVE_ITEMS_IN_FLASH_NUM		7

//DEV_PARAM_STORAGE_INFO  DT;
#define LABLE_MANNUFACTURE		1
#define LABLE_IV_STD			2
#define LABLE_IV_FACTOR			3
#define LABLE_ID				4
#define LABLE_PARAM_FILE		5
#define LABLE_ENCRYPT			6


typedef struct
{
	BPMS_AFN03_MANUV 		tgManuv;
	BPMS_AFNF0_VOL_CUR		tgIV;
	BPMS_AFNF0_RATIO    	tgIVFactor;
	BPMS_AFNF0_SERIAL_NUM 	tgId;
	FIRM_PARAM_FILE 		tgParamFile;
	STENCRYPT				tgEncrypt;
	
}DEV_PARAM_STORAGE_INFO;



extern const DEV_PARAM_STORAGE_INFO *extgdbdevGetDeviceSettingInfo(void);
extern const void* extgdbdevGetDeviceSettingInfoSt(u_char ubSTx);
extern int extgdbdevGetMeterUsedNum(void);
extern int extgdbdevSetDeviceSettingInfoSt(u_char ubSTx,u_short wOffset,const void*pBuf,u_short wSize);
#endif




