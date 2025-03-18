/* Embedded Systems - Exercise 12 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/nvic.h>
#include <stm32f4/exti.h>
#include <stm32f4/syscfg.h>
#include <stm32f4/tim.h>
#include <stm32f4/adc.h>


// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0
void init(){
	GPIOA_MODER = REP_BITS(GPIOA_MODER, 3 * 2, 2, GPIO_MODER_ANA);
	ADC1_SQR3 = 3;
	ADC1_CR1 = 0;
	ADC1_CR2 = ADC_ADON;
}

int main() {
	printf("\nStarting...\n");
    int off_threshold=2000;
	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB2ENR |= RCC_ADC1EN;
	GPIOD_MODER = REP_BITS(GPIOD_MODER , RED_LED*2 , 2 , GPIO_MODER_OUT) ;
	GPIOD_BSRR = 1 << (16 + 14);
    init();
	// initialization

	// main loop
	int a;
	printf("Endless loop!\n");
	while(1) {
		ADC1_CR2 |= ADC_SWSTART;
		while ((ADC1_SR & ADC_EOC) == 0);
		a = ADC1_DR;
		if (a>off_threshold){
			GPIOD_BSRR = 1 << (16 + 14);
		}
		else{
			GPIOD_BSRR = 1 << (+ 14);
		}
		printf("%d\n",a);

		
	}

}


