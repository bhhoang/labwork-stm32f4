/* Embedded Systems - Exercise 1 */

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


// Delay for debounce
#define DEBOUNCE 100000

// Button states
typedef enum {
    IDLE,
    PRESSED,
    RELEASED
} ButtonState;


int main() {
	printf("Starting...\n");

	// RCC init
	RCC_AHB1ENR |= RCC_GPIOAEN;
	RCC_AHB1ENR |= RCC_GPIODEN;

	// GPIO init
    GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2, 2, GPIO_MODER_IN);
    GPIOD_MODER = REP_BITS(GPIOD_MODER, GREEN_LED*2, 2, GPIO_MODER_OUT);

	ButtonState buttonState = IDLE;
    int ledState = 0;
    int debounceCounter = 0;

	printf("Endless loop!\n");

    while(1) {
        int currentButtonValue = (GPIOA_IDR & (1 << USER_BUT)) ? 1 : 0;

        switch(buttonState) {
            case IDLE:
                if (currentButtonValue) {
                    debounceCounter = DEBOUNCE;
                    buttonState = PRESSED;
                }
                break;

            case PRESSED:
                if (debounceCounter > 0) {
                    debounceCounter--;
                } else {
                    if (!currentButtonValue) {
                        buttonState = RELEASED;
                        ledState = !ledState;
                        if (ledState) {
                            GPIOD_ODR |= (1 << GREEN_LED);  // Turn on LED
                        } else {
                            GPIOD_ODR &= ~(1 << GREEN_LED); // Turn off LED
                        }
                        debounceCounter = DEBOUNCE;
                    }
                }
                break;

            case RELEASED:
                if (debounceCounter > 0) {
                    debounceCounter--;
                } else {
                    buttonState = IDLE;
                }
                break;
        }
    }
}
