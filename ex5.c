/* Embedded Systems - Exercise 5 */

#include <tinyprintf.h>
#include <stm32f4/rcc.h>
#include <stm32f4/gpio.h>
#include <stm32f4/tim.h>

// GPIOD
#define GREEN_LED	12
#define ORANGE_LED	13
#define RED_LED		14
#define BLUE_LED	15

// GPIODA
#define USER_BUT	0

// Timing definitions
#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC)  // 1 second
#define BLINK_PERIOD (WAIT_DELAY / 4) 

// Watchdog states
typedef enum {
    STATE_WAITING,  // Green LED on, waiting for timeout or button press
    STATE_BLINKING, // Red LED blinking, can be reset by button
    STATE_FROZEN    // Red LED permanently on, system frozen
} WatchdogState;


void init_timer() {
    TIM4_CR1 = 0;              
    TIM4_PSC = WAIT_PSC - 1;   
    TIM4_ARR = WAIT_DELAY;       
    TIM4_EGR = TIM_UG;          
    TIM4_SR = 0;                
    TIM4_CR1 = TIM_CEN;          
}

int main() {
    printf("\nStarting...\n");

    // RCC initialization
    RCC_AHB1ENR |= RCC_GPIOAEN; 
    RCC_AHB1ENR |= RCC_GPIODEN; 
    RCC_APB1ENR |= RCC_TIM4EN;  

    GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, RED_LED*2, 2, GPIO_MODER_OUT);
    
    // Set up button as input with pull-down
    GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2, 2, GPIO_MODER_IN);
    GPIOA_PUPDR = REP_BITS(GPIOA_PUPDR, USER_BUT*2, 2, GPIO_PUPDR_PD);
    
	init_timer();

    GPIOD_BSRR = 1 << GREEN_LED;
    
    // Variables
    WatchdogState state = STATE_WAITING;
    int button_was_pressed = 0;
    int blink_state = 0;
    int blink_count = 0;

    printf("Entering infinite loop!\n");
    
    while(1) {
        // Check button with debouncing
        int button_is_pressed = (GPIOA_IDR & (1 << USER_BUT)) != 0;
        
        // Button click detection (press and release)
        if (button_is_pressed && !button_was_pressed) {
            // Button just pressed
            button_was_pressed = 1;
        } 
        else if (!button_is_pressed && button_was_pressed) {
            // Button just released (click detected)
            button_was_pressed = 0;
            
            // Handle click based on current state
            if (state == STATE_WAITING) {
                // Reset timer
                TIM4_CNT = 0;
                TIM4_SR = 0;
                printf("Waiting timer reset\n");
            } 
            else if (state == STATE_BLINKING) {
                // Return to waiting state
                GPIOD_BSRR = 1 << (RED_LED + 16);  // Turn off red LED
                GPIOD_BSRR = 1 << GREEN_LED;       // Turn on green LED
                state = STATE_WAITING;
                TIM4_ARR = WAIT_DELAY;  // Restore 1s timeout
                TIM4_CNT = 0;
                TIM4_SR = 0;
                printf("Returned to waiting state\n");
            }
            // No action in FROZEN state
        }
        
        // State machine
        switch (state) {
            case STATE_WAITING:
                if (TIM4_SR & TIM_UIF) {
                    // 1 second timeout - switch to blinking
                    printf("5s timeout - switching to blinking\n");
                    GPIOD_BSRR = 1 << (GREEN_LED + 16);  // Turn off green LED
                    state = STATE_BLINKING;
                    TIM4_ARR = BLINK_PERIOD;  // 250ms period for blinking
                    blink_count = 0;
                    TIM4_SR = 0;
                }
                break;
                
            case STATE_BLINKING:
                if (TIM4_SR & TIM_UIF) {
                    TIM4_SR = 0;
                    
                    // Toggle red LED
                    if (blink_state) {
                        GPIOD_BSRR = 1 << (RED_LED + 16);  // Turn off
                        blink_state = 0;
                    } else {
                        GPIOD_BSRR = 1 << RED_LED;         // Turn on
                        blink_state = 1;
                    }
                    
                    // Count blinks
                    blink_count++;
                    if (blink_count >= 20) {  // 5 seconds (20 * 250ms)
                        printf("Blinking timeout - freezing\n");
                        state = STATE_FROZEN;
                        GPIOD_BSRR = 1 << RED_LED;  // Ensure red LED is on
                    }
                }
                break;
                
            case STATE_FROZEN:
                // System is frozen, nothing to do
                break;
        }
    }
}