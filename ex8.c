#include<tinyprintf.h>
#include<stm32f4/rcc.h>
#include<stm32f4/gpio.h>
#include<stm32f4/tim.h>
#include<stm32f4/nvic.h>

//GPIOD
#define GREEN_LED    12
#define ORANGE_LED   13
#define RED_LED      14
#define BLUE_LED     15

#define WAIT_PSC 1000
#define WAIT_DELAY (APB1_CLK / WAIT_PSC)
#define HALF_PERIOD (WAIT_DELAY/2)

//GPIODA
#define USER_BUT    0

void handle_TIM4(){
    TIM4_ARR = HALF_PERIOD;
    TIM4_SR = 0;
    TIM4_SR = 0;
    if((GPIOD_ODR & (1<<GREEN_LED))==0){
        printf("LED ON\n");
        GPIOD_BSRR = 1 << GREEN_LED;
    }
    else{
        printf("LED OFF\n");
        GPIOD_BSRR = 1 << (16 + GREEN_LED);
    }
    NVIC_ICPR(TIM4_IRQ >> 5) |= 1 << (TIM4_IRQ & 0X1f);
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
    TIM4_ARR=WAIT_DELAY;
    TIM4_EGR = TIM_UG;
    TIM4_SR = 0;
    //enableIRQ
    NVIC_ISER(TIM4_IRQ>>5)=1<<(TIM4_IRQ & 0X1f);
    TIM4_DIER=TIM_UIE;
    //startingall
    ENABLE_IRQS;
    TIM4_CR1 |= TIM_CEN;
}


int main(){
    printf("\nStarting...\n");

    //RCCinit
    RCC_AHB1ENR |= RCC_GPIOAEN;
    RCC_AHB1ENR |= RCC_GPIODEN;
    RCC_APB1ENR |= RCC_TIM4EN;
    RCC_APB2ENR |= RCC_ADC1EN;

    GPIOD_MODER = REP_BITS(GPIOD_MODER , GREEN_LED*2 , 2 , GPIO_MODER_OUT) ;
    GPIOA_MODER = REP_BITS(GPIOA_MODER, USER_BUT*2 ,2,GPIO_MODER_IN);

    configureTIM4();
    //mainloop
    printf("Endlessloop!\n");
    while(1){

    }

}