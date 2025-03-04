/* Embedded Systems - Exercise 1 */
#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>

// GPIOD
#define GREEN_LED 12
#define ORANGE_LED 13
#define RED_LED 14
#define BLUE_LED 15

// GPIODA
#define USER_BUT 0

void on(int led)
{
    GPIOD_BSRR = 1 << led;
}

void off(int led)
{
    GPIOD_BSRR = 1 << (led + 16);
}

int isOff(int led)
{
    return (GPIOD_ODR & (1 << led)) == 0;
}

#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC / 2)
int HALF_PERIOD = WAIT_DELAY;

void initTIM4()
{
    TIM4_CR1 = 0;
    TIM4_PSC = WAIT_PSC - 1;
    TIM4_ARR = HALF_PERIOD;
    TIM4_EGR = TIM_UG;
    TIM4_SR = 0;
    TIM4_CR1 = TIM_CEN;
}

int main()
{
    printf("Starting...\n");

    // RCC init
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_APB1ENR |= RCC_TIM4EN;

    // GPIO init
    for (int i = 0; i < 16; i++)
    {
        GPIOD_MODER = REP_BITS(GPIOD_MODER, i * 2, 2, GPIO_MODER_OUT);
        GPIOD_OTYPER = GPIOD_OTYPER & ~(1 << i);
    }
    GPIOA_MODER = REP_BITS(GPIOA_MODER, 0, 2, GPIO_MODER_IN);

    initTIM4();

    printf("Endless loop!\n");
    int times[] = {WAIT_DELAY * 2, WAIT_DELAY, WAIT_DELAY / 2};

    int time_state = 0;
    int button_state = 0;
    int last_b1 = 0;

    TIM4_CR1 = TIM_CEN;

    while (1)
    {
        while ((TIM4_SR & TIM_UIF) == 0)
        {
            if ((GPIOA_IDR & (1 << USER_BUT)) != 0)
            {
                // CLICK DETECTED
                button_state = 1;
                last_b1 = TIM4_CNT;
            }
            else if (button_state)
            {
                int now = TIM4_CNT;
                if (now <= last_b1)
                {
                    now += (WAIT_DELAY / 8);
                }
                if (now - last_b1 >= (WAIT_DELAY / 8))
                {
                    // CLICK AND RELEASE DETECTED, TRIGGER ACTION
                    //  action is to chqnge the time state variable
                    button_state = 0;
                    if (time_state == 2)
                    {
                        time_state = 0;
                    }
                    else
                    {
                        time_state += 1;
                    }
                    /*
                    BOTH VERSION WORK
                    printf("Setting HALF_PERIOD TO : %d \n", HALF_PERIOD / 4);
                    HALF_PERIOD /=4;*/
                    HALF_PERIOD = times[time_state];
                    printf("Setting HALF_PERIOD TO : %d \n", times[time_state]);
                }
            }
        }

        if (isOff(GREEN_LED))
        {
            // printf("%d ON", GREEN_LED);
            on(GREEN_LED);
        }
        else
        {
            // printf("%d OFF", GREEN_LED);
            off(GREEN_LED);
        }

        TIM4_ARR = HALF_PERIOD;
        TIM4_SR = 0;
    }
}