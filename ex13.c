/* Embedded Systems - Exercise 13 */

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
#define LED	3
#define PHOTO	3

#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC)
#define MS_PERIOD (WAIT_DELAY / 10)

// GPIODA
#define USER_BUT	0

volatile uint16_t adc_value = 0;

void ADC_IRQHandler(void) {
    if (ADC1_SR & ADC_EOC) {
        adc_value = ADC1_DR;  
        ADC1_SR &= ~ADC_EOC; 
    }
}
void handle_TIM4() {
	TIM4_SR = 0;
	ADC1_CR2 |= ADC_SWSTART;
}

void configureADC1() {
    DISABLE_IRQS;
    NVIC_ICER(ADC_IRQ >> 5) = 1 << (ADC_IRQ & 0x1F);
    NVIC_IRQ(ADC_IRQ) = (uint32_t)ADC_IRQHandler;
    NVIC_IPR(ADC_IRQ) = 0;
    NVIC_ICPR(ADC_IRQ >> 5) = 1 << (ADC_IRQ & 0x1F);
    NVIC_ISER(ADC_IRQ >> 5) = 1 << (ADC_IRQ & 0x1F);

	ADC1_CR1 = ADC_EOCIE ;
	ADC1_CR2 = ADC_ADON;
    ADC1_SQR3 = 3;
    ENABLE_IRQS;
}

void configureTIM4(){
	DISABLE_IRQS;//orjustdisabledevice
	//configureNVIC
	NVIC_ICER(TIM4_IRQ>>5)=1<<(TIM4_IRQ & 0X1f);
	NVIC_IRQ(TIM4_IRQ)=(uint32_t)handle_TIM4;
	NVIC_IPR(TIM4_IRQ)=0;
	//purgependingIRQ
	NVIC_ICPR(TIM4_IRQ>>5)=1<<(TIM4_IRQ & 0X1f);
	//configureTIM4
	TIM4_CR1 &= ~ TIM_CEN;
	TIM4_PSC=WAIT_PSC;
	TIM4_ARR=MS_PERIOD;
	TIM4_EGR = TIM_UG;
	TIM4_SR = 0;
	//enableIRQ
	NVIC_ISER(TIM4_IRQ>>5)=1<<(TIM4_IRQ & 0X1f);
	TIM4_DIER=TIM_UIE;
	//startingall
	ENABLE_IRQS;
	TIM4_CR1 |= TIM_CEN;
}


int main() {
	printf("\nStarting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;
	RCC_APB1ENR |= RCC_TIM4EN;
	RCC_APB2ENR |= RCC_ADC1EN;

	GPIOA_MODER = REP_BITS (GPIOA_MODER , PHOTO*2 , 2 , GPIO_MODER_ANA ) ;

	GPIOD_MODER = REP_BITS(GPIOD_MODER , LED *2 , 2 , GPIO_MODER_OUT) ;
	// initialization
	configureADC1();
	// main loop
	printf("Endless loop!\n");
	while(1) {
		ADC1_CR2 |= ADC_SWSTART; // important
		printf("%d\n",adc_value);
		if (adc_value > 2200) {
            GPIOD_BSRR = 1 << LED; 
        } else if (adc_value < 900) {
            GPIOD_BSRR = 1 << (LED + 16); 
        }
	}

}

