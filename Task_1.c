#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

void GPIO_Config_LED(void)
{
    GPIO_InitTypeDef gpio;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin   = GPIO_Pin_5;
    gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    GPIO_ResetBits(GPIOA, GPIO_Pin_5); 

void TIM2_Init_1MHz(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    RCC_ClocksTypeDef clk;
    RCC_GetClocksFreq(&clk);

    uint32_t timclk = clk.PCLK1_Frequency;
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
        timclk *= 2U;
    }

    uint16_t psc = (uint16_t)((timclk / 1000000U) - 1U); 

    TIM_TimeBaseInitTypeDef tb;
    tb.TIM_Prescaler         = psc;
    tb.TIM_CounterMode       = TIM_CounterMode_Up;
    tb.TIM_Period            = 0xFFFF;       
    tb.TIM_ClockDivision     = TIM_CKD_DIV1;
    tb.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tb);

    TIM_Cmd(TIM2, ENABLE);
}

void delay_us(uint16_t us)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < us) {;}
}

void delay_ms(uint32_t ms)
{
    while (ms--) delay_us(1000);
}

int main(void)
{
    SystemInit();
    GPIO_Config_LED();
    TIM2_Init_1MHz();

    while (1)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        delay_ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        delay_ms(500);
    }
}

