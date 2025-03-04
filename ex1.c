/* Embedded Systems - Exercise 1 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>



// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIOA
#define USER_BUT	0

int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIODEN;

	// GPIO init
	GPIOD_MODER = REP_BITS(GPIOD_MODER , GREEN_LED*2 , 2 , GPIO_MODER_OUT );
	GPIOD_MODER = REP_BITS(GPIOD_MODER , ORANGE_LED*2 , 2 , GPIO_MODER_OUT );
	GPIOD_MODER = REP_BITS(GPIOD_MODER , RED_LED*2 , 2 , GPIO_MODER_OUT );
	GPIOD_MODER = REP_BITS(GPIOD_MODER , BLUE_LED*2 , 2 , GPIO_MODER_OUT );

	printf("Endless loop!\n");

	while(1) {
		GPIOD_ODR = GPIOD_ODR | ( 1 << GREEN_LED );
		for(int i = 0; i < 30000000; i++) __asm__("nop");
		GPIOD_ODR = GPIOD_ODR & ~( 1 << GREEN_LED );

		GPIOD_ODR = GPIOD_ODR | ( 1 << ORANGE_LED );
		for(int i = 0; i < 30000000; i++) __asm__("nop");
		GPIOD_ODR = GPIOD_ODR & ~( 1 << ORANGE_LED );

		GPIOD_ODR = GPIOD_ODR | ( 1 << RED_LED );
		for(int i = 0; i < 30000000; i++) __asm__("nop");
		GPIOD_ODR = GPIOD_ODR & ~( 1 << RED_LED );

		GPIOD_ODR = GPIOD_ODR | ( 1 << BLUE_LED );
		for(int i = 0; i < 30000000; i++) __asm__("nop");
		GPIOD_ODR = GPIOD_ODR & ~( 1 << BLUE_LED );
	}

}
