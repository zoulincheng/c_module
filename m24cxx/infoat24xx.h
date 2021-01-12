#ifndef _INFOAT24_H
#define _INFOAT24_H

#include "bpmsp.h"

u_char halSave2AT24cxx(const DC_DC_BAT_PARAM *pfpf);
u_char halLoadFromAT24cxx(DC_DC_BAT_PARAM *pfpf);

u_char halLoadDcrefParmFromAT24cxx(DC_DC_REF_PARAM *pfpf);
u_char halSaveDcref2AT24cxx(const DC_DC_REF_PARAM *pfpf);

u_char halLoadDcdacParmFromAT24cxx(DC_DC_DAC_PARAM *pfpf);
u_char halSaveDcdac2AT24cxx(const DC_DC_DAC_PARAM *pfpf);

u_char halLoadBatteryParamFromAT24cxx(BPMS_AFN04_BATTERY_PARAM*pfpf);
u_char halSaveBatteryParam2AT24cxx(const BPMS_AFN04_BATTERY_PARAM*pfpf);

const BPMS_AFN03_MANUV *hal_get_made_info(void);


#define ADDR_BAT_PARAM			(0x00)		//[00-31]32 bytes
#define ADDR_REF_PARAM			(0x20)		//[32-63]32 bytes
#define ADDR_DAC_PARAM			(0x40)		//[64-95]32 bytes
#define ADDR_BATTERY_PARAM		(0xa0)		//
#define ADDR_CRC_SIZE			(2)
#endif
