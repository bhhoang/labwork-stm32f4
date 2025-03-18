/* Embedded Systems - Exercise 14 */

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
#define POTENTIOMETER  3   // Connected to PA3

// For tracking the potentiometer range
int min_value = 4095;
int max_value = 3240;

int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB2ENR |= RCC_ADC1EN;

	// GPIO initialization
	// Configure PA3 as analog input for potentiometer
	GPIOA_MODER = REP_BITS(GPIOA_MODER, POTENTIOMETER*2, 2, GPIO_MODER_ANA);
	
	// Configure LEDs as outputs (D12-D15)
	GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);   
	GPIOD_MODER = REP_BITS(GPIOD_MODER, ORANGE_LED*2, 2, GPIO_MODER_OUT);  
	GPIOD_MODER = REP_BITS(GPIOD_MODER, RED_LED*2, 2, GPIO_MODER_OUT);     
	GPIOD_MODER = REP_BITS(GPIOD_MODER, BLUE_LED*2, 2, GPIO_MODER_OUT);    
	
	// Configure push-pull for all LEDs
	GPIOD_OTYPER &= ~(1 << GREEN_LED);
	GPIOD_OTYPER &= ~(1 << ORANGE_LED);
	GPIOD_OTYPER &= ~(1 << RED_LED);
	GPIOD_OTYPER &= ~(1 << BLUE_LED);
	
	// Initially, all LEDs are off
	GPIOD_BSRR = (1 << (GREEN_LED + 16)) | (1 << (ORANGE_LED + 16)) | 
	            (1 << (RED_LED + 16)) | (1 << (BLUE_LED + 16));
	
	// Configure ADC1 to read PA3 (potentiometer)
	ADC1_SQR3 = POTENTIOMETER;    // Set channel 3 as first conversion
	ADC1_CR1 = 0;                 // 12-bit resolution (default)
	ADC1_CR2 = ADC_ADON;          // Turn on ADC
	
	printf("Endless loop - Vue-meter with potentiometer\n");
	printf("Rotate the potentiometer fully to calibrate min/max values\n");
	
	// Calibration period - allow user to rotate potentiometer to establish range
	for (int i = 0; i < 10; i++) {
		// Start ADC conversion
		ADC1_CR2 |= ADC_SWSTART;
		
		// Wait for conversion to complete
		while((ADC1_SR & ADC_EOC) == 0);
		
		// Get the ADC value and update min/max
		int value = ADC1_DR;
		if (value < min_value) min_value = value;
		if (value > max_value) max_value = value;
		
		// Small delay
		for (int j = 0; j < 1000000; j++) {
			asm("nop");
		}
	}
	
	printf("Calibrated range: min=%d, max=%d\n", min_value, max_value);
	
	// main loop
	while(1) {
		// Start ADC conversion
		ADC1_CR2 |= ADC_SWSTART;
		
		// Wait for conversion to complete
		while((ADC1_SR & ADC_EOC) == 0);
		
		// Get the ADC value
		int value = ADC1_DR;
		
		// Update min/max if necessary (continuous calibration)
		if (value < min_value) min_value = value;
		if (value > max_value) max_value = value;
		
		// Calculate normalized position (0-100%)
		int range = max_value - min_value;
		int normalized = range > 0 ? ((value - min_value) * 100) / range : 0;
		
		printf("Potentiometer: value=%d, percent=%d%%\n", value, normalized);
		
		// Turn LEDs on/off based on the normalized value
		// Initially turn all LEDs off
		GPIOD_BSRR = (1 << (GREEN_LED + 16)) | (1 << (ORANGE_LED + 16)) | 
		            (1 << (RED_LED + 16)) | (1 << (BLUE_LED + 16));
		
		// Then turn on the appropriate number of LEDs
		if (normalized >= 20) {   // 20% - first LED
			GPIOD_BSRR = (1 << GREEN_LED);
		}
		
		if (normalized >= 40) {   // 40% - second LED
			GPIOD_BSRR = (1 << ORANGE_LED);
		}
		
		if (normalized >= 60) {   // 60% - third LED
			GPIOD_BSRR = (1 << RED_LED);
		}
		
		if (normalized >= 80) {   // 80% - fourth LED
			GPIOD_BSRR = (1 << BLUE_LED);
		}
		
		// Add a small delay to avoid too frequent updates
		for (int i = 0; i < 500000; i++) {
			asm("nop");
		}
	}
}