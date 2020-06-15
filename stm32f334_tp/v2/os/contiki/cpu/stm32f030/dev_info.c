#include "contiki.h"
#include "basictype.h"
#include "sysprintf.h"
#include <string.h>


//#include "extgdb_376_2.h"

#include "eeflash.h"


//add meter header file
#include "dev_info.h"

//__attribute__((at(0x0805F000)))

extern int mem_cmp (const void* dst, const void* src, unsigned int cnt);

/*
//---start 校表参数文件-------（可根据计量芯片更改）
typedef struct sDl645FirmParaFile_TypeDef
{		
	u16			SYSCON;
	u16			EMUCON;
	u16			HFConst;
	u16			PStart;	
	u16			QStart;  			//10
	u16			GPQA;    	
	u16			GPQB;    	
	u16			IAGain;  	
	u16			UGain;   	
	u16			IBGain;  	
	u16			PhsA;	   	
	u16			PhsB;    	
	u16			QPhsCal; 			//22
	u16			APOSA;   	
	u16			APOSB;	 	
	u16			RPOSA;   	
	u16			RPOSB;   	
	u16			IARMSOS; 			//32
	u16			IBRMSOS;			//34
	u16			EMUCON2; 	
	float		KUrms;				// 电压系数
	float		KIArms;				// A通道电流系数
	float		KIBrms;				// B通道电流系数
	float		KPrms;				// 功率系数

	u32			urms;				// 电压系
	u32			arms;				// A通道系数
	u32			brms;				// B通道系数

	u32			adcKds;				// adc DS通道 校正系数
	u32			adcKv;				// adc v 通道 校正系数
	u8			verifyFlag;			// 0 DC not verify, 1 dc had verify
	
	u16			RealUI[2];			// 功率显示值，功率大于此值时显示 0.2%
	u32			RealPw;				// 电流显示值，电流大于此值时显示 0.2%
	u32			ChkSum;             	
	u16			RTCDota0;			// RTC校正寄存器
	u8			TemperAdj[2];		// 高低温补偿值
	u8			RTCAdj[4];			// RTC高低温时补偿值
	u8			CurAdj;				// 自热影响补偿值
	u8 			OfsetAdjAcVolt[2]; 	//根据电压调整OFFSET的值
	u16			CorrectionTemper;  	//校表时刻表计的温度
	u32			UCDCVerify;			//DC 校验修正值
	u32			IADCVerify;			//A 通道 DC电流校验修正值 检测流
	u32			IBDCVerify;			//B 通道 DC校验修正值，B通道用于检测电池电压
}FIRM_PARAM_FILE;		//58 Byte + 12
//---end 校表参数文件-------（可根据计量芯片更改） 
*/

const DEV_PARAM_STORAGE_INFO devInfo @0x0800F800 ={
//const DEV_PARAM_STORAGE_INFO devInfo = {
	//BPMS_AFN03_MANUV
	{
		{'J','Y'},
		{0x08,0x12,0x16},
		0x01
	},
	//BPMS_AFNF0_VOL_CUR
	{
		{0x00,0x10},   //
		{0x00,0x50}
	},
	//BPMS_AFNF0_RATIO
	{
		10000,		//default value 10000/10000=1.0
		10000
	},
	//BPMS_AFNF0_SERIAL_NUM
	{
		{0x11,0x12,0x13,0x14,0x15,0x16}
	},
	{
		0,				//SYSCON
		0,				//EMUCON
		0x2000,			//HFConst
		0x0060,			//PStart
		0x0120,			//QStart
		0x0,			//GPQA
		0x0,			//GPQB
		0x0,			//IAGain
		0x0,			//UGain
		0x0,			//IBGain
		0x0,			//PhsA
		0x0,			//PhsB
		0x0,			//QPhsCal
		0x0,			//APOSA
		0x0,			//APOSB
		0x0,			//RPOSA
		0x0,			//RPOSB
		0x0,			//IARMSOS
		0x0,			//IBRMSOS
		0x0,			//EMUCON2
		26.861,			//KUrms
		11.232,		//KIArms
		108.472,		//KIBrms
		1,				//KPrms
		
		26861,		//KUrms		xxx.yyy  xxx=number/1000  yyy=number%1000
		11232,		//KIArms	xxx.yyy  xxx=number/1000  yyy=number%1000
		108472,		//KIBrms	xxx.yyy	 xxx=number/1000  yyy=number%1000

		1000,		//adc DS 通道校正系数默认1.000
		1000,		//adc V  通道校正系数默认1.000
		0,			//dc verify flag 
		
		{
			0,			//RealUI[2]
			0
		},
		0,				//RealPw
		0,				//ChkSum
		0,				//RTCDota0
		{
			0,			//TemperAdj[2]
			0
		},
		{
			0,			//RTCAdj[4]
			0,
			0,
			0
		},
		0,				//CurAdj
		{
			0,			//OfsetAdjAcVolt[2]
			0
		},
		0,				//CorrectionTemper
		8944,			//UCDCVerify
		33300,			//IADCVerify
		33300			//IBDCVerify
	}
};

const DEV_PARAM_STORAGE_INFO defaultDevInfo  ={
//const DEV_PARAM_STORAGE_INFO devInfo = {
	//BPMS_AFN03_MANUV
	{
		{'J','Y'},
		{0x08,0x12,0x16},
		0x01
	},
	//BPMS_AFNF0_VOL_CUR
	{
		{0x00,0x10},   //
		{0x00,0x50}
	},
	//BPMS_AFNF0_RATIO
	{
		10000,		//default value 10000/10000=1.0
		10000
	},
	//BPMS_AFNF0_SERIAL_NUM
	{
		{0x11,0x12,0x13,0x14,0x15,0x16}
	},
	{
		0,				//SYSCON
		0,				//EMUCON
		0x2000,			//HFConst
		0x0060,			//PStart
		0x0120,			//QStart
		0x0,			//GPQA
		0x0,			//GPQB
		0x0,			//IAGain
		0x0,			//UGain
		0x0,			//IBGain
		0x0,			//PhsA
		0x0,			//PhsB
		0x0,			//QPhsCal
		0x0,			//APOSA
		0x0,			//APOSB
		0x0,			//RPOSA
		0x0,			//RPOSB
		0x0,			//IARMSOS
		0x0,			//IBRMSOS
		0x0,			//EMUCON2
		26.861,			//KUrms
		11.232,			//KIArms
		108.472,		//KIBrms
		1,				//KPrms
		
		26861,		//KUrms		xxx.yyy  xxx=number/1000  yyy=number%1000
		11232,		//KIArms	xxx.yyy  xxx=number/1000  yyy=number%1000
		108472,		//KIBrms	xxx.yyy	 xxx=number/1000  yyy=number%1000

		1000,		//adc DS 通道校正系数默认1.000
		1000,		//adc V  通道校正系数默认1.000
		0,			//dc verify flag 
		
		{
			0,			//RealUI[2]
			0
		},
		0,				//RealPw
		0,				//ChkSum
		0,				//RTCDota0
		{
			0,			//TemperAdj[2]
			0
		},
		{
			0,			//RTCAdj[4]
			0,
			0,
			0
		},
		0,				//CurAdj
		{
			0,			//OfsetAdjAcVolt[2]
			0
		},
		0,				//CorrectionTemper
		8944,			//UCDCVerify
		33300,			//IADCVerify
		33300			//IBDCVerify
	}
};

#if 0
//---start 校表参数文件-------（可根据计量芯片更改）
typedef struct sDl645FirmParaFile_TypeDef
{		
	u16			SYSCON;
	u16			EMUCON;
	u16			HFConst;
	u16			PStart;	
	u16			QStart;  			//10
	u16			GPQA;    	
	u16			GPQB;    	
	u16			IAGain;  	
	u16			UGain;   	
	u16			IBGain;  	
	u16			PhsA;	   	
	u16			PhsB;    	
	u16			QPhsCal; 			//22
	u16			APOSA;   	
	u16			APOSB;	 	
	u16			RPOSA;   	
	u16			RPOSB;   	
	u16			IARMSOS; 			//32
	u16			IBRMSOS;			//34
	u16			EMUCON2; 	
	float		KUrms;				// 电压系数
	float		KIArms;				// A通道电流系数
	float		KIBrms;				// B通道电流系数
	float		KPrms;				// 功率系数
	u16			RealUI[2];			// 功率显示值，功率大于此值时显示 0.2%
	u32			RealPw;				// 电流显示值，电流大于此值时显示 0.2%
	u32			ChkSum;             	
	u16			RTCDota0;			// RTC校正寄存器
	u8			TemperAdj[2];		// 高低温补偿值
	u8			RTCAdj[4];			// RTC高低温时补偿值
	u8			CurAdj;				// 自热影响补偿值
	u8 			OfsetAdjAcVolt[2]; 	//根据电压调整OFFSET的值
	u16			CorrectionTemper;  	//校表时刻表计的温度
	u32			UCDCVerify;			//DC 校验修正值
	u32			IADCVerify;			//A 通道 DC电流校验修正值 检测流
	u32			IBDCVerify;			//B 通道 DC校验修正值，B通道用于检测电池电压
}FIRM_PARAM_FILE;		//58 Byte + 12
//---end 校表参数文件-------（可根据计量芯片更改） 
#endif



static const void*pdevSettingBase[] = {
	&devInfo.tgManuv,
	&devInfo.tgIV,
	&devInfo.tgIVFactor,
	&devInfo.tgId,
	&devInfo.tgParamFile,
	&devInfo.tgEncrypt
};



const DEV_PARAM_STORAGE_INFO *extgdbdevGetDeviceSettingInfo(void)
{
	return (DEV_PARAM_STORAGE_INFO*)&devInfo;
}

/*
* \ brief Get param struct addr.
* \ param ubSTx Param struct lable num.
* \ return NULL, failed, other value 
*/
const void* extgdbdevGetDeviceSettingInfoSt(u_char ubSTx)
{
	if((ubSTx > 0) && (ubSTx<= MAX_SAVE_ITEMS_IN_FLASH_NUM))
	{

		ubSTx -= 1;

		return  pdevSettingBase[ubSTx];
	}	
	return NULL;
}


void extgdbSetDefaultSetting(void)
{
	u_short uwWriteWord=0;
	if (sizeof(DEV_PARAM_STORAGE_INFO)%(sizeof(u_long)) == 0)
	{
		uwWriteWord = (sizeof(DEV_PARAM_STORAGE_INFO)/sizeof(u_long));
	}
	else 
	{
		uwWriteWord = (sizeof(DEV_PARAM_STORAGE_INFO)/sizeof(u_long))+1;
	}

	eeWriteMultiWord(FLASH_USER_START_ADDR, (const u_long * )&defaultDevInfo, uwWriteWord);
	MEM_DUMP(6, "default", (void const *)FLASH_USER_START_ADDR, sizeof(DEV_PARAM_STORAGE_INFO));	
}



/*
* \ brief Write data to the addr
* \ param woffset offset addr
* \ param pBuf The pointer to the data
* \ param wSize Data size
* \ return -1 failed, 0 success.
*/

#if 0
int extgdbdevSetDeviceSettingInfoSt(u_char ubSTx,u_short wOffset,const void*pBuf,u_short wSize)
{
	//STENCRYPT tgEncrypt={0};
	//const STENCRYPT *pencrypt = (const STENCRYPT *)pdevSettingBase[6];

	if((ubSTx > 0) && (ubSTx<= 5))
	{
		ubSTx -= 1;
		XPRINTF((7,"ST:0x%08X+0x%04X[sz %d]\r\n",pdevSettingBase[ubSTx],wOffset,wSize));
		eeBlockWrite((u_long)pdevSettingBase[ubSTx]+wOffset,pBuf,wSize);
		/*
		if (pencrypt->encryptFlag != ENCRYPT_FLAG_SET)
		{
			encryptData(&tgEncrypt);
			eeBlockWrite((u_long)pdevSettingBase[6],(const void*)&tgEncrypt,sizeof(tgEncrypt));
		}
		*/
		MEM_DUMP(11, "FNFO", (void const *)FLASH_USER_START_ADDR, sizeof(DEV_PARAM_STORAGE_INFO));
		return 0;
	}
	return -1;
}
#else

static u_char ubanodeInfo[512];

int extgdbdevSetDeviceSettingInfoSt(u_char ubSTx,u_short wOffset,const void*pBuf,u_short wSize)
{
	if((ubSTx > 0) && (ubSTx<= MAX_SAVE_ITEMS_IN_FLASH_NUM))
	{
		DEV_PARAM_STORAGE_INFO *pdevInfo = (DEV_PARAM_STORAGE_INFO *)ubanodeInfo;
		//STENCRYPT tgEncrypt={0};
		
		u_short uwWriteWord = 0;
		STENCRYPT *pEnc;

		XPRINTF((12,"ST:0x%08X+0x%04X[sz %d]\r\n",pdevSettingBase[ubSTx-1],wOffset,wSize));
		//eeBlockRead((u_long)DEVINFO_FLASH_ADDR, &nodeInfo, sizeof(DEV_PARAM_STORAGE_INFO));
		eeBlockRead((u_long)FLASH_USER_START_ADDR, pdevInfo, sizeof(DEV_PARAM_STORAGE_INFO));
		//memcpy(pdevInfo, (void *)FLASH_USER_START_ADDR, sizeof(DEV_PARAM_STORAGE_INFO));
		if (ubSTx == LABLE_MANNUFACTURE)
		{
			pdevInfo->tgManuv = *(BPMS_AFN03_MANUV *)pBuf;
		}
		else if (ubSTx == LABLE_IV_STD)
		{
			pdevInfo->tgIV = *(BPMS_AFNF0_VOL_CUR*)pBuf;
		}
		else if (ubSTx == LABLE_IV_FACTOR)
		{
			pdevInfo->tgIVFactor = *(BPMS_AFNF0_RATIO*)pBuf;
		}
		else if (ubSTx == LABLE_ID)
		{
			pdevInfo->tgId  = *(BPMS_AFNF0_SERIAL_NUM*)pBuf;
		}
		else if (ubSTx == LABLE_PARAM_FILE)
		{
			pdevInfo->tgParamFile = *(FIRM_PARAM_FILE *)pBuf;
		}
		else if (ubSTx == LABLE_ENCRYPT)
		{
			//XPRINTF((0, "LA ENC\r\n"));
			pdevInfo->tgEncrypt = *(STENCRYPT*)pBuf;
		}

	
		if (pdevInfo->tgEncrypt.encryptFlag != ENCRYPT_FLAG_SET)
		{
			//encryptData( );
			//pEnc = (STENCRYPT *)encryptSt( );
			//pdevInfo->tgEncrypt = *pEnc;
		}
		
		
        //MEM_DUMP(6, "fNFO", &nodeInfo, sizeof(DEV_PARAM_STORAGE_INFO));
        //MEM_DUMP(9, "fNFO", pdevInfo, sizeof(DEV_PARAM_STORAGE_INFO));

		if (sizeof(DEV_PARAM_STORAGE_INFO)%(sizeof(u_long)) == 0)
		{
			uwWriteWord = (sizeof(DEV_PARAM_STORAGE_INFO)/sizeof(u_long));
		}
		else 
		{
			uwWriteWord = (sizeof(DEV_PARAM_STORAGE_INFO)/sizeof(u_long))+1;
		}
        //XPRINTF((11, "size=%d uw=%d, u_long = %d\r\n",sizeof(DEV_PARAM_STORAGE_INFO), uwWriteWord, sizeof(u_long)));
		//eeBlockWrite((u_long)pdevSettingBase[ubSTx]+wOffset,pBuf,wSize);
		//eeWriteMultiWord(DEVINFO_FLASH_ADDR, (const u_long * )&nodeInfo, (sizeof(DEV_PARAM_STORAGE_INFO)/sizeof(u_long)));
		eeWriteMultiWord(FLASH_USER_START_ADDR, (const u_long * )pdevInfo, uwWriteWord);
		MEM_DUMP(11, "FNFO", (void const *)FLASH_USER_START_ADDR, sizeof(DEV_PARAM_STORAGE_INFO));
		eeRDPSet( );
		return 0;
	}
	
	return -1;
}

#endif


//test mcu inter flash

#if 0
static MANNUFACTURE_PARAM_CONFIG stMake={
0x29,
0x32,
"abcde"
};
void extgdbTestFlash(void)
{
	const DEV_PARAM_STORAGE_INFO *pDevInfo = NULL;
	const MANNUFACTURE_PARAM_CONFIG *pMakerInfo = NULL;
	MANNUFACTURE_PARAM_CONFIG stMakerInfo = {0};
	u_long udwPaddr = NULL;
	
	pDevInfo = extgdbdevGetDeviceSettingInfo( );

	PRINTF("Protect is %02x\r\n", pDevInfo->udwProtectWord);
	PRINTF("Made info is %s\r\n", pDevInfo->st3MakerConfig.ubFirmMark);

	udwPaddr = (u_long)extgdbdevGetDeviceSettingInfoSt(1);
	PRINTF("ST1 addr is %08x\r\n", udwPaddr);


	udwPaddr = (u_long)extgdbdevGetDeviceSettingInfoSt(2);
	PRINTF("ST2 addr is %08x\r\n", udwPaddr);

	//test write flash
	PRINTF("Read st3 and print");
	udwPaddr = (u_long)extgdbdevGetDeviceSettingInfoSt(3);
	PRINTF("ST3 addr is %08x\r\n", udwPaddr);
	pMakerInfo = (MANNUFACTURE_PARAM_CONFIG *)udwPaddr;
	PRINTF("Softwarev is %02x\r\n", pMakerInfo->ubSoftWareVersion);
	PRINTF("Hardwarev is %02x\r\n", pMakerInfo->ubHardWareVersion);
	PRINTF("Hardwarev is %s\r\n", pMakerInfo->ubFirmMark);

	PRINTF("copy st3 and print");
	stMakerInfo = *pMakerInfo;
	PRINTF("Softwarev is %02x\r\n", stMakerInfo.ubSoftWareVersion);
	PRINTF("Hardwarev is %02x\r\n", stMakerInfo.ubHardWareVersion);
	PRINTF("Hardwarev is %s\r\n", stMakerInfo.ubFirmMark);


	PRINTF("write st3 and print");
	stMakerInfo.ubSoftWareVersion = 0x22;
	stMakerInfo.ubHardWareVersion = 0x33;

	extgdbdevSetDeviceSettingInfoSt(3, 0, &stMakerInfo, sizeof(MANNUFACTURE_PARAM_CONFIG));
	PRINTF("Softwarev is %02x\r\n", pMakerInfo->ubSoftWareVersion);
	PRINTF("Hardwarev is %02x\r\n", pMakerInfo->ubHardWareVersion);
	PRINTF("Hardwarev is %s\r\n", pMakerInfo->ubFirmMark);



	udwPaddr = (u_long)extgdbdevGetDeviceSettingInfoSt(4);
	PRINTF("ST4 addr is %08x\r\n", udwPaddr);
	
}

#endif



