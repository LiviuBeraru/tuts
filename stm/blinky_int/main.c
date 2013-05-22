/*
 * Make sure in system_stm32f4xx.c
 * #define PLL_M      8
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h" // RCC_AHB1PeriphClockCmd
#include "stm32f4xx_tim.h"
#include "misc.h"          // NVIC_Init

#define LEDS_N  4
/* Bit pattern pointing to all led pins*/
#define ALL_LED_PINS   (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)


static void init_leds(void);
static void init_timer(void);
static void init_interrupt(void);
static void delay(volatile uint32_t millis);

/* we toggle the leds in the order given in this array */
static const uint16_t led[LEDS_N] = {
/*  LD4, green    LD3, orange    LD5, red      LD6, blue   */
    GPIO_Pin_12 , GPIO_Pin_13 , GPIO_Pin_14 , GPIO_Pin_15
};

/* Index in the led array */
static uint32_t led_index     = 0;

/* Global counter being decremented in TIM7_IRQHandler.
   This counter is being set in the delay function. */
static uint32_t delay_counter = 0;


int main(void)
{
    init_leds();
    init_timer();
    init_interrupt();

    while (1)
    {
        led_index++;
        GPIO_ToggleBits(GPIOD, led[led_index % LEDS_N]);
        delay(1000);
    }

    return 0;
}

/* Initialize the leds. */
void init_leds(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_Pin   = ALL_LED_PINS;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(GPIOD, &gpio_init);
}

/* Initialize timer TIM7.
  
   See the notes in stm32f4xx_tim.c
   (TimeBase management functions) */
void init_timer(void)
{
    /* power on the TIM7 timer 
       (the microcontroller powers off every peripheral by default) */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    TIM_TimeBaseInitTypeDef timInitStruct;
    timInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    
    /* Don't modify clock frequency, 
     * we do this by setting the prescaler value*/
    timInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    
    /* According to the datasheet of the STM32F407xx
       (DM00037051.pdf, page 29, Table 3. Timer feature comparison)
       TIM7 runs at 84 MHz, that is it increments its value
       84000000 times in a second.
       By setting its prescaler to 1000 it increments only 
       84000 times in a second (1000 times slower).
       By setting its period (auto reload value)
       to 84, we make it increment 1000 times from 0 to 84 in a second.
       That is, it counts every millisecond from 0 to 84, then it interrupts.
       This way we get an interrupt every millisecond.
     */
    timInitStruct.TIM_Prescaler     = 1000;
    timInitStruct.TIM_Period        = 84;

    /* store this data into memory */
    TIM_TimeBaseInit(TIM7, &timInitStruct);
    /* enable the TIM7 interrupt source 
       this is not the same as enabling the actual interrupt routine,
       which we setup in init_interrupt(),
       it just enables TIM7 as interrupt source,
       that is it makes TIM7 emit interrupt signals
       when it overflows the TIM_Period value
     */
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    
    /* actually start the timer */
    TIM_Cmd(TIM7, ENABLE);
    
    /* From this point on, TIM7 runs and emits and interrupt signal
       every millisecond. Next step is to setup NVIC to actually
       catch the interrupt signal and branch to TIM7_IRQHandler. */
}

/* Initialize the TIM7 interrupt */
void init_interrupt(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* which interupt number */
    NVIC_InitStructure.NVIC_IRQChannel    = TIM7_IRQn;
    /* enable or disable? */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* lowest priority (highest is 0, lowest if 0x0F) */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0F;
    
    /* Store this data into NVIC memory map
       This function, which is declared in misc.h,
       computes a few values and stores them at the address NVIC, 
       which is #defined in core_cm4.h.
       NVIC is just a memory address casted to an NVIC
       configurations structure (NVIC_Type). */
    NVIC_Init(&NVIC_InitStructure);
    
    /* From this point on and having setup TIM7 in init_timer, 
       the interrupt handler TIM7_IRQHandler is being called 
       every millisecond. */
}

/* Interrupt handlder for the TIM7 event.

   The file startup_stm32f4xx.s stores the address of this
   function in the interrupt vector at the index corresponding
   to the TIM7 interrupt handler (.section .isr_vector) like this:
       .word     TIM7_IRQHandler
   Because of this fact, it is enough to name this function
   "TIM7_IRQHandler" in order to make it the interrupt handler
   for the TIM7 event.

   Because we setup TIM7 to interrupt
   every millisecond, this function is being called every
   millisecond. */
void TIM7_IRQHandler(void)
{
    if (delay_counter > 0) {
        delay_counter--;
    }
    
    /* Clear the TIM7 interrupt source or else the NVIC hardware
       sees the pending bit and thinks TIM7 is emiting the
       interupt again
       (mark this interrupt as handled) */
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
}

/* wait for millis milliseconds */
void delay(volatile uint32_t millis)
{
    delay_counter = millis;
    while (delay_counter > 0) {
        /* wait until delay_counter goes down to zero.
         * its value is being decremented every millisecond
         * by the interrupt handler TIM7_IRQHandler.
         * we use volatile so the compiler does not 
         * optimize away delay_counter and interpret
         * this loop as while (1);
         */
    }
}

