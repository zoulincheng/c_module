#include "stm32f4xx.h"
#include "arch_can.h"


#include "contiki.h"
#include "lib/ringbufindex.h"
#include "sysprintf.h"


PROCESS(can_txrx_test, "can_txrx");

#define CAN2_RX0_INT_ENABLE 1

u8 CAN2_Mode_Iint(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStucture;
	CAN_InitTypeDef CAN_InitStucture;
	CAN_FilterInitTypeDef CAN_FilterInitSturcture;

#if CAN2_RX0_INT_ENABLE
	NVIC_InitTypeDef NVIC_InitStructure;
#endif

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	
	GPIO_InitStucture.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStucture.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStucture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStucture.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStucture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStucture);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);

	GPIO_InitStucture.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStucture.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB,&GPIO_InitStucture);
	CAN_STB_SET(0);

	CAN_InitStucture.CAN_TTCM = DISABLE;
	CAN_InitStucture.CAN_ABOM = DISABLE;
	CAN_InitStucture.CAN_AWUM = DISABLE;
	CAN_InitStucture.CAN_NART = ENABLE;
	CAN_InitStucture.CAN_RFLM = DISABLE;
	CAN_InitStucture.CAN_TXFP = DISABLE;

	CAN_InitStucture.CAN_Mode = mode;
	CAN_InitStucture.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStucture.CAN_BS1 = CAN_BS1_7tq;
	CAN_InitStucture.CAN_BS2 = CAN_BS2_6tq;
	CAN_InitStucture.CAN_Prescaler = 6;

	CAN_Init(CAN1, &CAN_InitStucture);

	//CAN_Init(CAN2, &CAN_InitStucture);

	CAN_FilterInitSturcture.CAN_FilterNumber = 14;
	CAN_FilterInitSturcture.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitSturcture.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitSturcture.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitSturcture.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitSturcture.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitSturcture.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitSturcture.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitSturcture.CAN_FilterActivation = ENABLE;

	CAN_FilterInit(&CAN_FilterInitSturcture);
	
	//CAN_FilterInit(CAN2, &CAN_FilterInitSturcture);

#if CAN2_RX0_INT_ENABLE

	//CAN_ITConfig(CAN2, CAN_IT_TME,ENABLE);
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
	//CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
	
	//CAN_ITConfig(CAN2, CAN_IT_EWG, ENABLE);
	//CAN_ITConfig(CAN2, CAN_IT_EPV, ENABLE);
	//CAN_ITConfig(CAN2, CAN_IT_BOF, ENABLE);
	//CAN_ITConfig(CAN2, CAN_IT_LEC, ENABLE);
	//CAN_ITConfig(CAN2, CAN_IT_ERR, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_SCE_IRQn;
	NVIC_Init(&NVIC_InitStructure);


	
#endif
	return 0;
}


void CAN2_SCE_IRQHandler(void)
{
	if (CAN_GetFlagStatus(CAN2, CAN_IT_EWG))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_EWG);
		xprintf("CAN_IT_EWG\r\n");

	}

	if (CAN_GetFlagStatus(CAN2, CAN_IT_EPV))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_EPV);
		xprintf("CAN_IT_EPV\r\n");
	}	

	if (CAN_GetFlagStatus(CAN2, CAN_IT_BOF))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_BOF);
		xprintf("CAN_IT_BOF\r\n");
	}	

	if (CAN_GetFlagStatus(CAN2, CAN_IT_LEC))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_LEC);
		xprintf("CAN_IT_LEC\r\n");
	}	

	if (CAN_GetFlagStatus(CAN2, CAN_IT_ERR))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_ERR);
		xprintf("CAN_IT_ERR\r\n");
		
	}
#if 0
	if (CAN_GetFlagStatus(CAN2, CAN_IT_TME))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_TME);
		xprintf("CAN_IT_TME\r\n");
		
	}	
#endif 
	
	if (CAN_GetFlagStatus(CAN2, CAN_IT_FMP1))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
		xprintf("CAN_IT_FMP1\r\n");
		
	}	

	//xprintf("CAN2_SCE_IRQHandler\r\n");
	
}




#if CAN2_RX0_INT_ENABLE
/*
void CAN2_RX0_IRQHandler(void)
{
	int i = 0;
	CanRxMsg rxmessage;
	CAN_Receive(CAN2, 0, &rxmessage);
	xprintf("CAN2_RX0_IRQHandler\r\n");
	for (i = 0; i < 8; i++)
	{
		xprintf("i=%d, d=%02x", i, rxmessage.Data[i]);
	}

	xprintf("\r\n");

	if (CAN_GetFlagStatus(CAN2, CAN_IT_FMP1))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP1);
	}
}
*/

void CAN2_RX1_IRQHandler(void)
{
	int i = 0;
	CanRxMsg rxmessage;
	CAN_Receive(CAN2, 0, &rxmessage);
	xprintf("CAN2_RX1_IRQHandler\r\n");
	for (i = 0; i < 8; i++)
	{
		xprintf("i=%d, d=%02x", i, rxmessage.Data[i]);
	}

	xprintf("\r\n");
}
#endif


u8 CAN2_Send_Msg(u8 *msg, u8 len)
{
	u8 mbox;
	u16 i;
	CanTxMsg txmessage;
	txmessage.StdId = 0x12;
	txmessage.ExtId = 0x12;
	txmessage.IDE = 0;
	txmessage.RTR = 0;
	for (i = 0; i < len; i++)
	{
		txmessage.Data[i] = msg[i];
	}
	mbox = CAN_Transmit(CAN2, &txmessage);
	i = 0;
	while( (CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) &&(i<0xfff))i++;
	if (i >= 0xfff) return 1;
	return 0;
}

static  uint8_t tes1tdata[]={0x00, 0x01, 0x02, 0x03, 0x04};


PROCESS_THREAD(can_txrx_test, ev, data)
{
	static struct etimer _et;
	static CanTxMsg msg;
	PROCESS_BEGIN();
	while(1)
	{
		etimer_set(&_et, 1000);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
        CAN2_Send_Msg(tes1tdata,sizeof(tes1tdata));
	}
	PROCESS_END();
}


void can_test(void)
{
	CAN2_Mode_Iint(CAN_Mode_LoopBack);

	process_start(&can_txrx_test, NULL);
}


