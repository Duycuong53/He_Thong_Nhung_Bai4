#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "misc.h"

void GPIO_Config_LED(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin   = GPIO_Pin_5;          
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;     
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    GPIO_ResetBits(GPIOA, GPIO_Pin_5);     
}

void TIM2_Config_500ms_IT(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


    RCC_ClocksTypeDef clk;
    RCC_GetClocksFreq(&clk);

    uint32_t timclk = clk.PCLK1_Frequency;

    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
        timclk *= 2U;
    }


    uint16_t psc = (uint16_t)((timclk / 2000U) - 1U); 

    TIM_TimeBaseInitTypeDef tb;
    tb.TIM_Prescaler         = psc;
    tb.TIM_CounterMode       = TIM_CounterMode_Up;
    tb.TIM_Period            = 1000U - 1U;            
    tb.TIM_ClockDivision     = TIM_CKD_DIV1;
    tb.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tb);


    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);


    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = TIM2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);


    TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
 
        if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5) == Bit_SET)
            GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        else
            GPIO_SetBits(GPIOA, GPIO_Pin_5);

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
    }
}

int main(void)
{
    SystemInit();          
    GPIO_Config_LED();      
    TIM2_Config_500ms_IT(); 
    while (1)
    {

    }
}

