
void init_timer3_delay(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
    TIM_TimeBaseInit(TIM3, &(TIM_TimeBaseInitTypeDef) {
        .TIM_Prescaler = (72-1),                                                        // 预分频器
         .TIM_CounterMode = TIM_CounterMode_Down,                                       // 向下计数
          .TIM_Period = (1000-1),                                                       // 设置自动重载值
           .TIM_ClockDivision = TIM_CKD_DIV1,                                           // 时钟分频
            .TIM_RepetitionCounter = 0                                                  // 此处无意义
    });
 
    while((TIM3->SR & TIM_FLAG_Update)!=SET);                                           // 计时到时，此处为1
    TIM3->SR = (uint16_t)~TIM_FLAG_Update;
}
//
 
void delay_us(unsigned int us)
{
    TIM3->CNT = us-1;
    TIM3->CR1 |= TIM_CR1_CEN;                                                           // 使能计数器
    while((TIM3->SR & TIM_FLAG_Update)!=SET);
    TIM3->SR = (uint16_t)~TIM_FLAG_Update;
    TIM3->CR1 &= ~TIM_CR1_CEN;
}


延迟1us：

 /*******************************************************************************
 * 函 数 名         : delay_us
 * 函数功能           : 延时函数，延时us
 * 输    入         : i
 * 输    出         : 无
 *******************************************************************************/
 void delay_us(u32 i)
 {
     u32 temp;
     SysTick->LOAD=9*i;          			//设置重装数值, 72MHZ时
     SysTick->CTRL=0X01;         			//使能，减到零是无动作，采用外部时钟源
     SysTick->VAL=0;                		//清零计数器
     do
     {
         temp=SysTick->CTRL;           		//读取当前倒计数值
     }
     while((temp&0x01)&&(!(temp&(1<<16)))); //等待时间到达
     SysTick->CTRL=0;    					//关闭计数器
     SysTick->VAL=0;        				//清空计数器
}


 

延迟1ms：

 /*******************************************************************************
 * 函 数 名         : delay_ms
 * 函数功能           : 延时函数，延时ms
 * 输    入         : i
 * 输    出         : 无
 *******************************************************************************/
 void delay_ms(u32 i)
 {
     u32 temp;
     SysTick->LOAD=9000*i;      			//设置重装数值, 72MHZ时
     SysTick->CTRL=0X01;        			//使能，减到零是无动作，采用外部时钟源
     SysTick->VAL=0;            			//清零计数器
     do
     {
         temp=SysTick->CTRL;       			//读取当前倒计数值
     }
     while((temp&0x01)&&(!(temp&(1<<16)))); //等待时间到达
     SysTick->CTRL=0;    					//关闭计数器
     SysTick->VAL=0;        				//清空计数器
 }
 

注意：以上两函数中间的参数u32 i不能超过1800，举例，想定时一分钟，可以通过for循环让delay_ms（1000）走60次，
而不能使用delay_ms（60000），不然程序就出错了。




3.代码：使PG14，每隔一秒闪烁1次

#include <stm32f10x.h>

void GPIO_Config(void);
void NVIC_Config(void);
void TIMER_Config(void);

int main(void)
{
    GPIO_Config();
    NVIC_Config();
    TIMER_Config();
    
    while(1);
}

void GPIO_Config(void)
{
    /* 定义GPIO结构体 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 总线2使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
    
    /* 配置GPIO引脚，模式，速度 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* 初始化 */
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void NVIC_Config(void)
{
    /* 定义配置中断向量表变量 */
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 选择中断分组1 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    /* 选择TIM2的中断通道 */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    
    /* 抢占式中断优先级设置为0 */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    
    /* 响应式中孤单优先级设置为0 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    
    /* 使能中断 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    
    /* 初始化 */
    NVIC_Init(&NVIC_InitStructure);
}

/* 定时器的配置 */
void TIMER_Config(void)
{
    /* 定义TIM的变量 */
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    /* 开启总线1 */
    /* TIM2~7 在总线1上 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    /* 设置TIM2为默认设置 */
    TIM_DeInit(TIM2);
    
    /* 自动重装载寄存器的值 */
    TIM_TimeBaseStructure.TIM_Period = 2000 - 1;
    /* 时钟预分频数 */
    TIM_TimeBaseStructure.TIM_Prescaler = (36000 - 1);//36000000/36000
    /* 采样分频 */
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    /* 向上计数模式 */
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    /* 初始化 */
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    
    /* 清除溢出中断标志 */
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    
    /* 开启TIM2的中断 */
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    /* 开启时钟 */
    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
    u8 ReadValue;
    
    /* 检测是否发生溢出更新事件 */
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        /* 清除TIM2的中断待处理位 */
        TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
        
        ReadValue = GPIO_ReadOutputDataBit(GPIOG, GPIO_Pin_14);
        
        /* 若该端口当前为低电平 */
        if(ReadValue == 0)
        {
            GPIO_SetBits(GPIOG, GPIO_Pin_14);
        }
        else
        {
            GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        }
    }
}
 

转载于:https://www.cnblogs.com/maplesnow/p/5380721.html


#include "time.h"
#include "led.h"
void time3_init(u16 per,u16 pre)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能TIM3的时钟
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_CKD_DIV1是.h文件中已经定义好的，TIM_CKD_DIV1=0，也就是时钟分频因子为0
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//计数方式为向上计数
    TIM_TimeBaseStructure.TIM_Period=per;//周期
    TIM_TimeBaseStructure.TIM_Prescaler=pre;//分频系数
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能TIM3的更新中断
    TIM_Cmd(TIM3,ENABLE);//使能TIM3
}

#include "time.h"
#include "led.h"
void time3_init(u16 per,u16 pre)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能TIM3的时钟
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//TIM_CKD_DIV1是.h文件中已经定义好的，TIM_CKD_DIV1=0，也就是时钟分频因子为0
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//计数方式为向上计数
    TIM_TimeBaseStructure.TIM_Period=per;//周期
    TIM_TimeBaseStructure.TIM_Prescaler=pre;//分频系数
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//使能TIM3的更新中断
    TIM_Cmd(TIM3,ENABLE);//使能TIM3
}
void NVIC_INIT(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);//设定中断优先级分组0
    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;//设定中断向量   本程序为TIM3的中断
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能响应中断向量的中断响应
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//配置中断向量的抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;//配置中断向量的响应优先级
    NVIC_Init(&NVIC_InitStructure);//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void TIM3_IRQHandler(void) //TIME3中断服务函数  需要设定中断优先级  即NVIC配置
{
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)//判断是否发生了更新(溢出)中断
    {
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除溢出中断标志位
    }

    LED0=!LED0;
}



__set_PRIMASK(1);          //关总中断


__set_PRIMASK(0);          //开总中断

