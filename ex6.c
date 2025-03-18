/* Embedded Systems - Exercise 6 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>
#include <stm32f4/nvic.h>

// GPIOD
#define GREEN_LED    12
#define ORANGE_LED   13
#define RED_LED      14
#define BLUE_LED     15

// GPIODA
#define USER_BUT     0

#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC) // 1 second

volatile int TIM4_triggered = 0;

void handle_TIM4() {
    TIM4_triggered = 1;
    TIM4_SR &= ~TIM_UIF;
}

int main() {
    printf("\nStarting...\n");
    
    // RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_APB1ENR |= RCC_TIM4EN;
    
    // GPIO init
    GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
    GPIOD_OTYPER &= ~(1 << GREEN_LED);
    
    GPIOD_BSRR = 1 << (GREEN_LED + 16);
    
    DISABLE_IRQS;
    
    NVIC_ICER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0x1f);
    NVIC_IRQ(TIM4_IRQ) = (uint32_t)handle_TIM4;
    NVIC_IPR(TIM4_IRQ) = 0;
    
    NVIC_ICPR(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0x1f);
    
    // Configure TIM4
    TIM4_CR1 = 0;             
    TIM4_PSC = WAIT_PSC - 1;  
    TIM4_ARR = WAIT_DELAY / 2;
    TIM4_EGR = TIM_UG;        
    TIM4_SR = 0;              
    
    TIM4_DIER = TIM_UIE;
    NVIC_ISER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0x1f);
    
    ENABLE_IRQS;

	TIM4_CR1 = TIM_CEN;
    
    printf("Endless loop!\n");
    
    // Main loop
    while(1) {
        if (TIM4_triggered) {
            TIM4_triggered = 0;
            
            if ((GPIOD_ODR & (1 << GREEN_LED)) == 0) {
                GPIOD_BSRR = 1 << GREEN_LED;
                printf("LED ON\n");
            } else {
                GPIOD_BSRR = 1 << (GREEN_LED + 16);
                printf("LED OFF\n");
            }
        }
    }
}