#include "stm32f4xx.h"
#include "arch_can.h"


#include "contiki.h"
#include "lib/ringbufindex.h"
#include "sysprintf.h"

static struct ringbufindex ri;
static const uint8_t ri_size = CAN_RINGINDEX_NUM;
static CanRxMsg tgCanMsg[CAN_RINGINDEX_NUM];
CanRxMsg RxMessage;
CanTxMsg TxMessage;

PROCESS(can_rx_process, "can_rx");
PROCESS(can_tx_test, "can_txt");


const CanTxMsg canCfg = {
	.StdId = 0x321,
	.ExtId = 0x01,
	.RTR = CAN_RTR_DATA,
	.IDE = CAN_ID_STD,
};

static void _can_init(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
//	CanTxMsg TxMessage;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;


	/*can rx interrupt configuration*/
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	/* CAN GPIOs configuration **************************************************/
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

	/* Connect CAN pins to AF9 */
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 

	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = CAN_STB_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN_STB_PORT, &GPIO_InitStructure);
	CAN_STB_SET( 0 );
	
	/* CAN configuration ********************************************************/  
	/* Enable CAN clock */
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);   
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);   
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
 	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);   
	/* CAN register init */
	CAN_DeInit(CANx);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

/*
	CAN_InitStucture.CAN_Mode = mode;
	CAN_InitStucture.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStucture.CAN_BS1 = CAN_BS1_7tq;
	CAN_InitStucture.CAN_BS2 = CAN_BS2_6tq;
	CAN_InitStucture.CAN_Prescaler = 6;
*/

/*
对于stm32 can的时钟为fpclk = 30m，我所期望的波特率为baud = 500k。
那么根据公式 1/baud= （BRP +1）（BS1+BS2+3）*fpclk,算出（BRP +1）（BS1+BS2+3） = 60。
那么取BRP = 2，得到BS1+BS2 = 17。然后设置最佳采样点 令 (1+1+BS1)/(1+1+1+BS1+BS2) = 87.5%。
最后得出 BRP = 2 , BS1 = 15,BS2 = 2.
设置如下：
*/
	/* CAN Baudrate = 1 MBps (CAN clocked at 30 MHz) */
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_7tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
	CAN_InitStructure.CAN_Prescaler = 6;
	CAN_Init(CANx, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 14;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* Transmit Structure preparation */
	TxMessage.StdId = 0x321;
	TxMessage.ExtId = 0x01;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.DLC = 1;

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


/**
  * @brief  Initializes the Rx Message.
  * @param  RxMessage: pointer to the message to initialize
  * @retval None
  */
void canInit_RxMes(CanRxMsg *RxMessage)
{
	uint8_t ubCounter = 0;

	RxMessage->StdId = 0x00;
	RxMessage->ExtId = 0x00;
	RxMessage->IDE = CAN_ID_STD;
	RxMessage->DLC = 0;
	RxMessage->FMI = 0;
	for (ubCounter = 0; ubCounter < 8; ubCounter++)
	{
		RxMessage->Data[ubCounter] = 0x00;
	}
}


void _cantx_cfg(const CanTxMsg *pcfg, CanTxMsg *pmsg)
{
	//	TxMessage.RTR = CAN_RTR_DATA;
	//TxMessage.IDE = CAN_ID_STD;
	pmsg->StdId = pcfg->StdId;
	pmsg->ExtId = pcfg->ExtId;
	pmsg->RTR = pcfg->RTR;
	pmsg->IDE = pcfg->IDE;
}


/*
	u8 mbox;
	u16 i;

	mbox = CAN_Transmit(CAN2, &txmessage);
	i = 0;
	while( (CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) &&(i<0xfff))i++;

*/

void canTransmit(const CanTxMsg *pcfg, const void *pdata, int len)
{
	CanTxMsg TxMessage; 
	const uint8_t *pd=(const uint8_t *)pdata;
	uint8_t mbox;
	uint16_t i;
	int slen = 0;
	if (len < 0 || pdata == NULL)
		return;
		
	slen = len;
	_cantx_cfg(pcfg, &TxMessage);
	
	while(slen > 8)
	{
		XPRINTF((8, "slen1 = %d\r\n", slen));
		TxMessage.DLC = 8;
		memcpy(TxMessage.Data, pd, 8);
		slen -= 8;
		pd += 8;
		#if 0
		CAN_Transmit(CANx, &TxMessage);
		XPRINTF((8, "slen1 = %d\r\n", slen));
		/* Wait until one of the mailboxes is empty */
		while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
		      (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
		      (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
		#else
		mbox = CAN_Transmit(CAN2, &TxMessage);
		i = 0;
		while( (CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) &&(i<0xfff))i++;
		#endif
	}

	XPRINTF((8, "slen2 = %d\r\n", slen));
	TxMessage.DLC = slen;
	memcpy(TxMessage.Data, pd, slen);
#if 0	
	CAN_Transmit(CANx, &TxMessage);
	/* Wait until one of the mailboxes is empty */
	while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
	      (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
	      (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
#endif
	mbox = CAN_Transmit(CAN2, &TxMessage);
	i = 0;
	while( (CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) &&(i<0xfff))i++;
}


void canSendData(const void *pdata, int len)
{
	canTransmit(&canCfg, pdata, len);
}


/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
#if 1
void CAN2_RX0_IRQHandler(void)
{
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
	int index = 0;
	xprintf("can20\r\n");

	//CAN_GetITStatus(CAN2, 0xffffffff)
	//uint32_t itflag = 
	//if (CAN_GetFlagStatus(CAN2, CAN_IT_FMP0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		xprintf("CAN_IT_FMP1\r\n");
		
	}	
	if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD))
	{
		xprintf("can21\r\n");
		#if 1
		index = ringbufindex_peek_put(&ri);
		if (index >= 0)
		{
			xprintf("can22\r\n");
			tgCanMsg[index] = RxMessage; //add data to buf
			ringbufindex_put(&ri);
			process_poll(&can_rx_process);
		}
		#endif
	}
	//canInit_RxMes(&RxMessage);
}

#endif

PROCESS_THREAD(can_rx_process, ev, data)
{
	CanRxMsg _msg;
	int index;
	PROCESS_BEGIN();
	while(1)
	{
		PROCESS_YIELD();
		if (ev == PROCESS_EVENT_POLL)
		{
			index = ringbufindex_get(&ri);
			if (index >= 0)
			{
				_msg = tgCanMsg[index];
				XPRINTF((8, "index = %d\r\n", index));
				MEM_DUMP(8, "crx", _msg.Data, _msg.DLC);
			}
		}
		if (!ringbufindex_empty(&ri))
		{
			process_poll(&can_rx_process);
		}
	}
	
	PROCESS_END();
}


const uint8_t testdata[]={0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0f};


PROCESS_THREAD(can_tx_test, ev, data)
{
	static struct etimer _et;
	PROCESS_BEGIN();
	while(1)
	{
		etimer_set(&_et, 1000);
		PROCESS_YIELD_UNTIL(etimer_expired(&_et));
		//canSendData(testdata, sizeof(testdata));
		//MEM_DUMP(10, "CTX", testdata, sizeof(testdata));
        TxMessage.Data[0] = 1;
        CAN_Transmit(CANx, &TxMessage);
        /* Wait until one of the mailboxes is empty */
        while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
              (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
              (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
		
	}
	PROCESS_END();
}



void testcan(void)
{
	uint8_t mbox;
	uint16_t i;

    TxMessage.Data[0] = 1;
    #if 0
    CAN_Transmit(CANx, &TxMessage);
    /* Wait until one of the mailboxes is empty */
    while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
          (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
          (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
    #else
	mbox = CAN_Transmit(CAN2, &TxMessage);
	i = 0;
	while( (CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) &&(i<0xfff))i++;
    #endif
}

void testcan1(void)
{
	canSendData(testdata, sizeof(testdata));
}

void canInit(void)
{
	ringbufindex_init(&ri, CAN_RINGINDEX_NUM);
	_can_init( );
	
	process_start(&can_rx_process, NULL);
	//process_start(&can_tx_test, NULL);
}

