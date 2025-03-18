/* Embedded Systems - Exercise 15 */

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

// SERVO PA6
#define SERVO_PIN	6

// PWM settings for servo control
#define PWM_PSC         83          
#define PWM_PERIOD      10000       
#define PULSE_MIN       433
#define PULSE_MAX       1800        

#define WAVE_PSC        4199
#define WAVE_PERIOD     99

volatile int current_step = 0;
volatile int direction = 1;   // 1 = up, -1 = down

void handle_TIM4() {
    current_step += direction;
    
    if (current_step >= 100) {
        current_step = 100;
        direction = -1;
    } else if (current_step <= 0) {
        current_step = 0;
        direction = 1;
    }
    
    int pulse_width = PULSE_MIN + (current_step * (PULSE_MAX - PULSE_MIN)) / 100;
    
    TIM3_CCR1 = pulse_width;
    
    TIM4_SR &= ~TIM_UIF;
}

void init_servo() {
    GPIOA_MODER &= ~(3 << (SERVO_PIN*2));             // Clear mode bits
    GPIOA_MODER |= (GPIO_MODER_ALT << (SERVO_PIN*2)); // Set alternate function mode
    
    // Set alternate function 2 (TIM3)
    GPIOA_AFRL &= ~(0xF << (SERVO_PIN*4));            // Clear AF bits
    GPIOA_AFRL |= (2 << (SERVO_PIN*4));               // Set AF2
    
    // Configure TIM3 for PWM
    TIM3_CR1 = 0;                // Stop timer during configuration
    TIM3_PSC = PWM_PSC;          // Set prescaler for 500KHz
    TIM3_ARR = PWM_PERIOD;       // Set period to 20ms
    
    // Configure channel 1 for PWM mode
    TIM3_CCMR1 = TIM_OC1M_PWM1;  // PWM mode 1 on channel 1
    TIM3_CCER = TIM_CC1E;        // Enable channel 1 output
    
    // Set initial position (0°)
    TIM3_CCR1 = PULSE_MIN;
    
    // Generate update event
    TIM3_EGR = TIM_UG;
    
    // Start timer
    TIM3_CR1 |= TIM_CEN;
    
    printf("Servo initialized (PWM: %dHz, pulse: %d-%dµs)\n", 
           500000/PWM_PERIOD, PULSE_MIN*2, PULSE_MAX*2);
}

// Initialize TIM4 for flag waving motion
void init_wave_timer() {
    // Disable interrupts during setup
    DISABLE_IRQS;
    
    // Configure NVIC for TIM4
    NVIC_ICER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0x1f);
    NVIC_IRQ(TIM4_IRQ) = (uint32_t)handle_TIM4;
    NVIC_IPR(TIM4_IRQ) = 0;
    NVIC_ICPR(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0x1f);
    
    // Configure TIM4 for 10ms interrupts
    TIM4_CR1 = 0;                // Stop timer
    TIM4_PSC = WAVE_PSC;         // Set prescaler
    TIM4_ARR = WAVE_PERIOD;      // Set period
    TIM4_DIER = TIM_UIE;         // Enable update interrupt
    TIM4_EGR = TIM_UG;           // Generate update event
    TIM4_SR = 0;                 // Clear status register
    
    // Enable TIM4 interrupt in NVIC
    NVIC_ISER(TIM4_IRQ >> 5) = 1 << (TIM4_IRQ & 0x1f);
    
    // Start TIM4
    TIM4_CR1 |= TIM_CEN;
    
    // Re-enable interrupts
    ENABLE_IRQS;
    
    printf("Wave timer started (period: 2s)\n");
}

// Initialize status LED
void init_led() {
    // Configure green LED as output
    GPIOD_MODER &= ~(3 << (GREEN_LED*2));             // Clear mode bits
    GPIOD_MODER |= (GPIO_MODER_OUT << (GREEN_LED*2)); // Set output mode
    
    // Turn on green LED
    GPIOD_BSRR = 1 << GREEN_LED;
}

int main() {
    printf("\nStarting...\n");

    // RCC init - enable required clocks
    RCC_AHB1ENR |= RCC_GPIOAEN;  // Enable GPIOA clock
    RCC_AHB1ENR |= RCC_GPIODEN;  // Enable GPIOD clock
    RCC_APB1ENR |= RCC_TIM3EN;   // Enable TIM3 clock for PWM
    RCC_APB1ENR |= RCC_TIM4EN;   // Enable TIM4 clock for timing
    RCC_APB2ENR |= RCC_ADC1EN;   // Enable ADC1 clock

    // Initialize components
    init_led();          // Initialize status LED
    init_servo();        // Initialize servo PWM
    init_wave_timer();   // Start the waving motion

    // Main loop
    printf("Endless loop!\n");
    while(1) {
        // Flag waving is handled in TIM4 interrupt
    }
}