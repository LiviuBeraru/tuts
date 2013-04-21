/*
 * This program turns on the 4 leds of the stm32f4 discovery board
 * one after another.
 * It defines shortcut definitions for the led pins and
 * stores the order of the leds in an array which is being
 * iterated in a loop.
 *
 * This program is free human culture like poetry, mathematics
 * and science. You may use it as such.
 */

/* stm32f4_discovery.h is located in Utilities/STM32F4-Discovery
 * and defines the GPIO Pins where the leds are connected.
 * Including this header also includes stm32f4xx.h and
 * stm32f4xx_conf.h, which includes stm32f4xx_gpio.h
 */
#include "stm32f4_discovery.h"

/* We make code more readable by giving the led-pins
 * symbolic names depending on the led color.
 * The values LED4_PIN etc are defined in stm32f4_discovery.h
 * as GPIO_Pin_12 etc.
 */
#define GREEN  LED4_PIN
#define ORANGE LED3_PIN
#define RED    LED5_PIN
#define BLUE   LED6_PIN
#define ALL_LEDS (GREEN | ORANGE | RED | BLUE) // all leds

/* This is how long we wait in the delay function. */
#define PAUSE_LONG  4000000L
#define PAUSE_SHORT 1000000L

/* The GPIO port where the leds are connected
 * (same pin numbers are present on many GPIO ports).
 * Leds are connected to pins 12 through 15 on GPIO port D,
 * so we use the port GPIOD.
 * GPIOD is just a memory address casted to a GPIO_TypeDef pointer
 * GPIO_TypeDef is defined in stm32f4xx.h
 */
#define LEDS_GPIO_PORT (GPIOD)

/* This array stores the led order used to switch them on and off.
 * We use this order by iterating over the array.
 * LEDn is the number of user leds on the discovery board
 * and is defined in stm32f4_discovery.h.
 */
static uint16_t leds[LEDn] = {GREEN, ORANGE, RED, BLUE};


/* Structure storing the information used to intialize
 * some GPIO port, in our case the port D, where the 
 * leds are connected.
 * It is declared in stm32f4xx_gpio.h.
 */
GPIO_InitTypeDef GPIO_InitStructure;


/* A simple time consuming function.
 * For a more real-world one,
 * we would use timers and interrupts. */
static void delay(__IO uint32_t nCount)
{
    while(nCount--)
        __asm("nop"); // do nothing
}

/* Initialize the GPIOD port.
 * See also the comments beginning stm32f4xx_gpio.c
 * (found in the library)
 */
static void setup_leds(void)
{
    /* Enable the GPIOD peripheral clock before we
     * actually setup GPIOD.
     * This function is declared in stm32f4xx_rcc.h
     * and implemented in stm32f4xx_rcc.c
     */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* which pins we select to setup
     * every pin number is mapped to a bit number */
    GPIO_InitStructure.GPIO_Pin   = ALL_LEDS;
    /* pins in output mode */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    /* high clock speed for the selected pins
     * see stm32f4xx_gpio.h for different speeds
     * (enum GPIOSpeed_TypeDef) */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    /* operating output type for the selected pins
     * see the enum GPIOOType_TypeDef in stm32f4xx_gpio.h
     * for different values.
     * PP stands for "push/pull", OD stands for "open drain"
     * google for "push pull vs open drain" */
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    /* operating Pull-up/Pull down for the selected pins */
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    /* Write this data into memory at the address
     * mapped to GPIO device port D, where the led pins
     * are connected */
    GPIO_Init(LEDS_GPIO_PORT, &GPIO_InitStructure);
    /* This call resolves in
     * GPIO_Init((GPIO_TypeDef *) 0X40020C00, &GPIO_InitStructure)
     * where 0X40020C00 is the memory address mapped to
     * the GPIOD port. Without the library we would have to know all
     * these memory addresses. */
}

/* Turn on the color leds one after another.
 * The order of the leds is defined in the array leds above.
 * The functions GPIO_SetBits and GPIO_ResetBits are declared
 * in stm32f4xx_gpio.h and implemented in stm32f4xx_gpio.c.
 *
 * You might want to look at the implementation in order to see
 * how this works.
 * Basically, this works by using the memory map mechanism of the
 * Cortex-M4: the pins of the GPIO port D are mapped to special
 * memory addresses which these function write to.
 * The exact addresses are represented by the fields of the
 * GPIO_TypeDef structure (that is by their offsets).
 * See also the structure GPIO_TypeDef in stm32f4xx.h.
 */
static void led_round(void)
{
    int i;
    for (i = 0; i < LEDn; i++)
    {
        /* turn on led */
        GPIO_SetBits(LEDS_GPIO_PORT, leds[i]);
        /* wait a while */
        delay(PAUSE_LONG);
        /* turn off all leds */
        GPIO_ResetBits(LEDS_GPIO_PORT, ALL_LEDS);
    }
}

/* Turn all leds on and off 4 times. */
static void flash_all_leds(void)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        /* Turn on all user leds */
        GPIO_SetBits(LEDS_GPIO_PORT, ALL_LEDS);
        /* Wait a short time */
        delay(PAUSE_SHORT);
        /* Turn off all leds */
        GPIO_ResetBits(LEDS_GPIO_PORT, ALL_LEDS);
        /* Wait again before looping */
        delay(PAUSE_SHORT);
    }
}

/* Main function, the entry point of this program.
 * The main function is called from the startup code in file
 * Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s
 * (line 101)
 */
int main(void)
{
    setup_leds();
    while (1)
    {
        led_round();
        flash_all_leds();
    }

    return 0; // never returns actually
}
