/* Embedded Systems - Exercise 7 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC)
#define HALF_PERIOD (WAIT_DELAY/2)

// Button state
volatile int b1_state = 0;
volatile int last_b1 = 0;
#define DELAY_50MS (WAIT_DELAY / 20)

void init_TIM4(){
    TIM4_CR1 = 0;
    TIM4_PSC = WAIT_PSC - 1;
    TIM4_ARR = WAIT_DELAY;
    TIM4_EGR = TIM_UG; 
    TIM4_SR = 0;
    TIM4_CR1 = TIM_CEN;
}
void handle_button() {
    // Check if button is pressed (high) or released (low)
    if ((GPIOA_IDR & (1 << USER_BUT)) != 0) {
        // Button is pushed
        b1_state = 1;
        last_b1 = TIM4_CNT;
    } else if (b1_state) {
        // Button is released after being pushed
        int now = TIM4_CNT;
            
    // Configure TIM4 for debouncing
        if (now <= last_b1)
            now += DELAY_50MS;
            
        // If sufficient time has passed (debouncing)
        if (now - last_b1 >= DELAY_50MS) {
            b1_state = 0;
            
            // Toggle LED
            if ((GPIOD_ODR & (1 << GREEN_LED)) == 0)
                GPIOD_BSRR = 1 << GREEN_LED;  // Turn on
            else
                GPIOD_BSRR = 1 << (GREEN_LED + 16);  // Turn off
        }
    }
    
    // Reset the interrupt
    EXTI_PR |= 1 << USER_BUT;  // Clear pending bit in EXTI
    NVIC_ICPR(EXTI0_IRQ >> 5) |= 1 << (EXTI0_IRQ & 0x1F);  // Clear pending in NVIC
}

int main() {
    printf("\nStarting...\n");

    // RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;
    RCC_APB2ENR |= (1 << 14);
    
    // Configure LED
    GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);  // Output
    GPIOD_OTYPER &= ~(1 << GREEN_LED);  // Push-pull
    
    // Configure button
    GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2, 2, GPIO_MODER_IN);  // Input
    GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, USER_BUT*2, 2, GPIO_PUPDR_PD);  // Pull-down


    init_TIM4();

    // Configure EXTI for button
    DISABLE_IRQS;
    
    // Configure EXTI to select GPIOA (0)
    SYSCFG_EXTICR1 = REP_BITS(SYSCFG_EXTICR1, 0, 4, 0);
    
    // Configure EXTI line 0
    EXTI_RTSR |= 1 << USER_BUT;  // Rising edge
    EXTI_FTSR |= 1 << USER_BUT;  // Falling edge
    EXTI_IMR |= 1 << USER_BUT;   // Enable interrupt
    EXTI_PR |= 1 << USER_BUT;    // Clear pending
    
    // Configure NVIC
    NVIC_ICER(EXTI0_IRQ >> 5) = 1 << (EXTI0_IRQ & 0x1F);
    NVIC_IRQ(EXTI0_IRQ) = (uint32_t)handle_button;
    NVIC_IPR(EXTI0_IRQ) = 0;
    NVIC_ICPR(EXTI0_IRQ >> 5) = 1 << (EXTI0_IRQ & 0x1F);
    NVIC_ISER(EXTI0_IRQ >> 5) = 1 << (EXTI0_IRQ & 0x1F);
    
    ENABLE_IRQS;

    // main loop
    printf("Endless loop!\n");
    while(1) {
        // Main program continues here
    }
}