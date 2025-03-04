/* Embedded Systems - Exercise 2 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0


int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;

	// GPIO init
	GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2, 2, GPIO_MODER_IN);
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
	printf("Endless loop!\n");
	while(1) {
        if (GPIOA_IDR & (1 << USER_BUT)) {
            GPIOD_ODR |= (1 << GREEN_LED);
        } else {
            GPIOD_ODR &= ~(1 << GREEN_LED);
        }
    }

}
