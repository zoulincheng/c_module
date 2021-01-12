#include "basictype.h"
#include "sysprintf.h"
#include "m24cxx.h"
//#include "rn8209d.h"
#include "bytetoword.h"
#include "bpmsp.h"

#include "infoat24xx.h"


/*
//---start 校表参数文件-------（可根据计量芯片更改）
typedef struct _dc_dc_ref_param
{	
	int p_buck_v;	//power default buck v
	int p_boost_v;  //power default boost v
	int b_buck_v;   //battery default buck v
	int b_boost_v;	//battery default boost v
}DC_DC_REF_PARAM;	//58 Byte + 12

typedef struct _dc_dc_m_param
{
	int type;			//battery type
	int ah;				//
	int avgv;			//const charge v
	int floatv;			//float charge v
	int charge_factor;
	int discharge_factor;
}DC_DC_M_PARAM;
*/
const DC_DC_BAT_PARAM tgParamFile = {
	1,
	300,
	564,
	535,
	10,
	10,
};

const DC_DC_REF_PARAM tg_ref = {
	4810,		//p_buck_v
	4832,
	4810,
	4835,
	2495,
};

const DC_DC_DAC_PARAM tg_dac = {
		78,				//
		87,				//
};


const BPMS_AFN03_MANUV tgmanv = {
	{'J','Y'},					//factory
	{0x21,0x01,0x11},			//year month date
	0x30						//version
};

/*
*typedef struct{
	u_char type;
	u_char offLineSw;
	u_char batteryAH[2];
	u_char chargeV[2];
	u_char floatV[2];
	u_char chargeFactor[2];
	u_char dischargeFactor[2];
}BPMS_AFN04_BATTERY_PARAM;
*/
const BPMS_AFN04_BATTERY_PARAM tgBparam={
	.type 			= BAT_TYPE_NONE,				//
	.offLineSw 		= 1,
	.batteryAH 		= {0x2c, 0x01},			//300AH
	.chargeV 		= {0x08, 0x16},			//56.40V
	.floatV 		= {0xc8, 0x14},			//53.20
	.chargeFactor	= {0x0a, 0x00},			//0.10
	.dischargeFactor= {0x5a, 0x00}, 		//0.90
};

/*
 *\brief Load the Module Configuration from MC24C02
 *
 *\param bModNum, module object index
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halLoadFromAT24cxx(DC_DC_BAT_PARAM *pfpf)
{
	#if 1
    u_short wCRC;
    u_char  bResult = 0;
    u_short size = sizeof(DC_DC_BAT_PARAM);
    at24cxx_read(ADDR_BAT_PARAM,&wCRC,ADDR_CRC_SIZE);
    at24cxx_read(ADDR_BAT_PARAM+ADDR_CRC_SIZE,pfpf,size);
    bResult = (wCRC == CRC16((u_char*)pfpf,size));
        
    #if I2C_DUMP_OPTIOIN
    {
        char szTitle[8];
        
        xsprintf(szTitle,"%c_%04X",bResult?'L':'E',wCRC);
        MEM_DUMP(10, szTitle, pfpf, size);
    }
    #endif
        
    if(!bResult){
        *pfpf = tgParamFile;
        XPRINTF((0, "load default file-------tgParamFile-------------------\r\n"));
    }
    
    return bResult;
    #else
    memcpy(pfpf, &tgParamFile, sizeof(FIRM_PARAM_FILE));
    return TRUE;
    #endif
}

/*
 *\brief save the Module Configuration to MC24C02
 *
 *\param bModNum, module object index
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halSave2AT24cxx(const DC_DC_BAT_PARAM *pfpf)
{
    FIRM_FPAR tgpMP = {0x00};
    u_short wCRC;
    u_long size = sizeof(DC_DC_BAT_PARAM);
    u_short sized = sizeof(FIRM_FPAR);
    
    wCRC = CRC16((u_char*)pfpf,size);
    at24cxx_read(ADDR_BAT_PARAM,&tgpMP,sized);
	XPRINTF((0, "-%04X  -%04X \r\n", wCRC,make_word(tgpMP.baCRC[0],tgpMP.baCRC[1])));
    
    if(wCRC != make_word(tgpMP.baCRC[0],tgpMP.baCRC[1]) || mem_cmp(&tgpMP.baMP[0],pfpf,size) != 0){
        tgpMP.baCRC[0] = BYTE_LW(wCRC);
        tgpMP.baCRC[1] = BYTE_HI(wCRC);
        memcpy(&tgpMP.baMP[0],pfpf,size);
        at24cxx_write(ADDR_BAT_PARAM,&tgpMP,sized);

        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"S_%04X",wCRC);
            MEM_DUMP(10, "save", tgpMP.baMP, size);
        }
        #endif
    }else{
    	XPRINTF((0, "no need to write to MC24C02\r\n"));
        //no need to write to MC24C02
        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"N_%04X",wCRC);
            MEM_DUMP(10, szTitle, pfpf, size);
        }
        #endif
    }
    return 1;
}

void halSaveDefaultCfg(void)
{
	halSave2AT24cxx(&tgParamFile);
}

const BPMS_AFN03_MANUV *hal_get_made_info(void)
{
	return &tgmanv;
}

/*
 *\brief Load the Module Configuration from MC24C02
 *
 *\param bModNum, module object index
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halLoadDcrefParmFromAT24cxx(DC_DC_REF_PARAM *pfpf)
{
	#if 1
    u_short wCRC;
    u_char  bResult = 0;
    u_long  size = sizeof(DC_DC_REF_PARAM);
    
    at24cxx_read( ADDR_REF_PARAM,&wCRC,ADDR_CRC_SIZE);
    at24cxx_read( ADDR_REF_PARAM+ADDR_CRC_SIZE,pfpf,size);
    bResult = (wCRC == CRC16((u_char*)pfpf,size));
        
    #if I2C_DUMP_OPTIOIN
    {
        char szTitle[8];
        
        xsprintf(szTitle,"%c_%04X",bResult?'L':'E',wCRC);
        MEM_DUMP(10, szTitle, pfpf, size));
    }
    #endif
        
    if(!bResult){
        *pfpf = tg_ref;
        XPRINTF((0, "load default file-------tg_ref-------------------\r\n"));
    }
    
    return bResult;
    #else
    memcpy(pfpf, &tgParamFile, sizeof(FIRM_PARAM_FILE));
    return TRUE;
    #endif
}
/*
 *\brief save the Module Configuration to MC24C02
 *
 *\param bModNum, module object index
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halSaveDcref2AT24cxx(const DC_DC_REF_PARAM *pfpf)
{
    REF_FPAR tgpMP = {0x00};
    u_short wCRC;
    u_long  size = sizeof(DC_DC_REF_PARAM);
    u_short sizetem = sizeof(REF_FPAR);
    
    wCRC = CRC16((u_char*)pfpf,size);
    
    at24cxx_read(ADDR_REF_PARAM,&tgpMP,sizetem);

	XPRINTF((0, "-%04X  -%04X \r\n", wCRC,make_word(tgpMP.baCRC[0],tgpMP.baCRC[1])));
    
    if(wCRC != make_word(tgpMP.baCRC[0],tgpMP.baCRC[1]) || mem_cmp(&tgpMP.baMP[0],pfpf,size) != 0){
        tgpMP.baCRC[0] = BYTE_LW(wCRC);
        tgpMP.baCRC[1] = BYTE_HI(wCRC);
        memcpy(&tgpMP.baMP[0],pfpf,size);
        at24cxx_write(ADDR_REF_PARAM,&tgpMP,sizetem);

        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"S_%04X",wCRC);
            MEM_DUMP(10, "save", tgpMP.baMP,size);
        }
        #endif
    }else{
    	XPRINTF((0, "no need to write to MC24C02\r\n"));
        //no need to write to MC24C02
        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"N_%04X",wCRC);
            MEM_DUMP(10, szTitle, pfpf, size);
        }
        #endif
    }
    return 1;
}


/*
 *\brief Load the Module Configuration from MC24C02
 *
 *\param bModNum, module object index
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halLoadDcdacParmFromAT24cxx(DC_DC_DAC_PARAM *pfpf)
{
	#if 1
    u_short wCRC;
    u_char  bResult = 0;
    u_long  size = sizeof(DC_DC_DAC_PARAM);
    
    at24cxx_read( ADDR_DAC_PARAM,&wCRC,ADDR_CRC_SIZE);
    at24cxx_read( ADDR_DAC_PARAM+ADDR_CRC_SIZE,pfpf,size);
    bResult = (wCRC == CRC16((u_char*)pfpf,size));
        
    #if I2C_DUMP_OPTIOIN
    {
        char szTitle[8];
        
        xsprintf(szTitle,"%c_%04X",bResult?'L':'E',wCRC);
        MEM_DUMP(10, szTitle, pfpf, size));
    }
    #endif
        
    if(!bResult){
        *pfpf = tg_dac;
        XPRINTF((0, "load default file-------tg_dac-------------------\r\n"));
    }
    
    return bResult;
    #else
    memcpy(pfpf, &tgParamFile, sizeof(FIRM_PARAM_FILE));
    return TRUE;
    #endif
}
/*
 *\brief save the Module Configuration to MC24C02
 *
 *\param bModNum, module object index
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halSaveDcdac2AT24cxx(const DC_DC_DAC_PARAM *pfpf)
{
    DAC_FPAR tgpMP = {0x00};
    u_short wCRC;
    u_long  size = sizeof(DC_DC_DAC_PARAM);
    u_short sizetem = sizeof(DAC_FPAR);
    
    wCRC = CRC16((u_char*)pfpf,size);
    
    at24cxx_read(ADDR_DAC_PARAM,&tgpMP,sizetem);

	XPRINTF((0, "-%04X  -%04X \r\n", wCRC,make_word(tgpMP.baCRC[0],tgpMP.baCRC[1])));
    
    if(wCRC != make_word(tgpMP.baCRC[0],tgpMP.baCRC[1]) || mem_cmp(&tgpMP.baMP[0],pfpf,size) != 0){
        tgpMP.baCRC[0] = BYTE_LW(wCRC);
        tgpMP.baCRC[1] = BYTE_HI(wCRC);
        memcpy(&tgpMP.baMP[0],pfpf,size);
        at24cxx_write(ADDR_DAC_PARAM,&tgpMP,sizetem);

        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"S_%04X",wCRC);
            MEM_DUMP(10, "save", tgpMP.baMP,size);
        }
        #endif
    }else{
    	XPRINTF((0, "no need to write to MC24C02\r\n"));
        //no need to write to MC24C02
        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"N_%04X",wCRC);
            MEM_DUMP(10, szTitle, pfpf, size);
        }
        #endif
    }
    return 1;
}





/*
 *\brief Load the BATTERY Module Configuration from MC24C02
 *
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halLoadBatteryParamFromAT24cxx(BPMS_AFN04_BATTERY_PARAM*pfpf)
{
	#if 1
    u_short wCRC;
    u_char  bResult = 0;
    u_short size = sizeof(BPMS_AFN04_BATTERY_PARAM);
    
    at24cxx_read( ADDR_BATTERY_PARAM,&wCRC,ADDR_CRC_SIZE);  //160
    at24cxx_read((ADDR_BATTERY_PARAM+ADDR_CRC_SIZE),pfpf,size);
    bResult = (wCRC == CRC16((u_char*)pfpf,size));
        
    #if I2C_DUMP_OPTIOIN
    {
        char szTitle[8];
        
        xsprintf(szTitle,"%c_%04X",bResult?'L':'E',wCRC);
        MEM_DUMP(10, szTitle, pfpf,size);
    }
    #endif
        
    if(!bResult){
        *pfpf = tgBparam;
        XPRINTF((0, "load default file--------tgBparam------------------\r\n"));
    }
    
    return bResult;
    #else
    memcpy(pfpf, &tgParamFile, sizeof(FIRM_PARAM_FILE));
    return TRUE;
    #endif
}

/*
 *\brief save the Module Battery param Configuration to MC24C02
 *
 *\param pfpf, module configuration object
 *
 * return  0 failed, 1 suceeds
 */
u_char halSaveBatteryParam2AT24cxx(const BPMS_AFN04_BATTERY_PARAM*pfpf)
{
    BATTERY_FPAR tgpMP = {0x00};
    u_short wCRC;
    u_long size = sizeof(BPMS_AFN04_BATTERY_PARAM);
    u_short sized = sizeof(BATTERY_FPAR);
    wCRC = CRC16((u_char*)pfpf,size);

    at24cxx_read(ADDR_BATTERY_PARAM,&tgpMP,sized);

	XPRINTF((0, "-%04X  -%04X \r\n",wCRC, make_word(tgpMP.baCRC[0],tgpMP.baCRC[1])));
    
    if(wCRC != make_word(tgpMP.baCRC[0],tgpMP.baCRC[1]) || mem_cmp(&tgpMP.baMP[0],pfpf,size) != 0){
        tgpMP.baCRC[0] = BYTE_LW(wCRC);
        tgpMP.baCRC[1] = BYTE_HI(wCRC);
        memcpy(&tgpMP.baMP[0],pfpf,size);
        at24cxx_write(ADDR_BATTERY_PARAM,&tgpMP,sized);

        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"S_%04X",wCRC);
            MEM_DUMP(10, "save", tgpMP.baMP, size);
        }
        #endif
    }else{
    	XPRINTF((0, "no need to write to MC24C02\r\n"));
        //no need to write to MC24C02
        #if I2C_DUMP_OPTIOIN
        {
            char szTitle[8];
            xsprintf(szTitle,"N_%04X",wCRC);
            MEM_DUMP(10, szTitle, pfpf, size);
        }
        #endif
    }
    return 1;
}


void testBatteryParam(void)
{
	u_char btype;
	u_char offLineSw;
	u_short batAH;
	u_short chargeV;
	u_short floatV;
	u_short chargeFactor;
	u_short dischargeFactor;
	BPMS_AFN04_BATTERY_PARAM tgB;
	
	halSaveBatteryParam2AT24cxx(&tgBparam);
	halLoadBatteryParamFromAT24cxx(&tgB);

	btype = tgB.type;
	offLineSw = tgB.offLineSw;
	batAH = *((u_short*)&tgB.batteryAH[0]);
	chargeV = *((u_short*)&tgB.chargeV[0]);
	floatV =*((u_short*)&tgB.floatV[0]);
	chargeFactor = *((u_short*)&tgB.chargeFactor[0]);
	dischargeFactor = *((u_short*)&tgB.dischargeFactor[0]);

	XPRINTF((0, "bt=%d, sw=%d, ah=%d, cv=%d, fv= %d, cf=%d,df=%d\r\n", btype, offLineSw, batAH, chargeV, floatV, chargeFactor, dischargeFactor));
}


void testModuleBatParam(void)
{
	DC_DC_BAT_PARAM  tgB = {0};
	
	halSave2AT24cxx(&tgParamFile);
	halLoadFromAT24cxx(&tgB);

	XPRINTF((0, "type =%d, ah=%d, avgv=%d, fv=%d, c_f= %d, d_f=%d\r\n", tgB.type, tgB.ah, tgB.avgv, tgB.floatv, tgB.c_f, tgB.d_f));
}

void testDcrefParam(void)
{
	DC_DC_REF_PARAM  tgB = {0};
	
	halSaveDcref2AT24cxx(&tg_ref);
	halLoadDcrefParmFromAT24cxx(&tgB);

	XPRINTF((0, "p_bk_v =%d, p_bt_v=%d, b_bk_v=%d, b_bt_v=%d\r\n", tgB.p_buck_v, tgB.p_boost_v, tgB.b_buck_v, tgB.b_boost_v));
}

void testDcdacParam(void)
{
	DC_DC_DAC_PARAM  tgB = {0};
	
	halSaveDcdac2AT24cxx(&tg_dac);
	halLoadDcdacParmFromAT24cxx(&tgB);

	XPRINTF((0, "pf =%d, bf=%d\r\n", tgB.pf, tgB.bf));
}


void test_print_param(void)
{
	//BPMS_AFN04_BATTERY_PARAM tgB1={0};
	DC_DC_BAT_PARAM  tgB2 = {0};
	DC_DC_REF_PARAM  tgB3 = {0};
	DC_DC_DAC_PARAM  tgB4 = {0};

	//halLoadBatteryParamFromAT24cxx(&tgB);
	halLoadFromAT24cxx(&tgB2);	
	halLoadDcrefParmFromAT24cxx(&tgB3);	
	halLoadDcdacParmFromAT24cxx(&tgB4);

	//XPRINTF((0, "bt=%d, sw=%d, ah=%d, cv=%d, fv= %d, cf=%d,df=%d\r\n", btype, offLineSw, batAH, chargeV, floatV, chargeFactor, dischargeFactor));
	XPRINTF((0, "type =%d, ah=%d, avgv=%d, fv=%d, c_f= %d, d_f=%d\r\n", tgB2.type, tgB2.ah, tgB2.avgv, tgB2.floatv, tgB2.c_f, tgB2.d_f));


	XPRINTF((0, "p_bk_v =%d, p_bt_v=%d, b_bk_v=%d, b_bt_v=%d\r\n", tgB3.p_buck_v, tgB3.p_boost_v, tgB3.b_buck_v, tgB3.b_boost_v));

	XPRINTF((0, "pf =%d, bf=%d\r\n", tgB4.pf, tgB4.bf));
}


void hal_print_dac(DC_DC_DAC_PARAM *pd)
{
	XPRINTF((0, "pf =%d, bf=%d\r\n", pd->pf, pd->bf));
}

void hal_print_ref(DC_DC_REF_PARAM *pd)
{
	XPRINTF((0, "p_bk_v =%d, p_bt_v=%d, b_bk_v=%d, b_bt_v=%d, vref=%d\r\n", pd->p_buck_v, pd->p_boost_v, pd->b_buck_v, pd->b_boost_v, pd->vref2_5));
}






/*
void at24xtest2(void)
{
	u_char buf[128] = {0x00};
	at24cxx_write(0, &tgParamFile, sizeof(tgParamFile));
	MEM_DUMP(0, "TESW", &tgParamFile, sizeof(tgParamFile));
	at24cxx_read(0x00,buf,sizeof(tgParamFile));
	MEM_DUMP(0, "TESR", buf, sizeof(tgParamFile));
}
*/
