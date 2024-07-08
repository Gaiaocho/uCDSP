/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "../CMSIS/Inc/main.h"
#include "../CMSIS/Inc/stm32f407xx.h"

//for print f
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define BLUE_LED_PORT GPIOD
#define BLUE_LED_PIN 15

Lis3_Config_t Accel_1;


volatile uint8_t SHOULD_READ = 1;



static void ConfigureTim7(void);




int main(void)
{	
	//Setup Systick so we remove delays
	Timer_Init();
	ConfigureTim7();
	





	//Blue LED CMSIS
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER15_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_1;

	GPIOD->BSRR |= GPIO_BSRR_BS_15;


	
	//enable FPU
	SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));

	/*I2c*/
	uint32_t rcc_speed =   RCC_GetPCLK1Value();

	printf("System booted with Default clock of: %d MHz \n", rcc_speed);


	//Set up pins
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 

	uint32_t altfn_reg;

	//Setup I2c
	GPIOB->MODER |= GPIO_MODER_MODER6_1;
	GPIOB->OTYPER |= (1 << GPIO_OTYPER_OT6_Pos);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_1;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6_0;
	//get this pins altfn reg
	altfn_reg = 6;
	GPIOA->AFR[0] |= 4 << (altfn_reg * 4);

	GPIOB->MODER |= GPIO_MODER_MODER7_1;
	GPIOB->OTYPER |= (1 << GPIO_OTYPER_OT7_Pos);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_1;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7_0;
	//get this pins altfn reg
	altfn_reg = 7;
	GPIOA->AFR[0] |= 4 << (altfn_reg * 4);

	printf("GPIO setup on port b pin 6 scl pin 7 sda \n");

	/*end i2c*/


	/*LIS3 Setup*/
    //choose datarate, filter, axes
	Accel_1.Lis3_DR = 0x60;
	Accel_1.Lis3_BDU = 0x0;
	Accel_1.Lis3_Axes = 0x7;
	Accel_1.Lis3_Sensitivity = SENSITIVITY_2G;
	/* END LIS3 Setup*/

	/*ADC SETup*/

	AdcInit();

	/* End ADC SETUP */

	

	/*SPI Read Write*/
	uint8_t read_address = 0x0F;
	uint8_t read_data = 0xAA;
	uint8_t read_settings = 0xFF;
	uint8_t ctrl_4 = 0x20;
	
	Lis3_Init(Accel_1);
	
	Lis3WriteRead(read_address, &read_data);
	printf("ID Data: %#X \n", read_data);
	Lis3WriteRead(ctrl_4, &read_settings);
	printf("ODR and Axes Config Data: %#X \n", read_settings);
	/*End SPI Read Write*/
	
	//Make a timer for main 
	DelayTimer_t mainTimer;
	Timer_Start(&mainTimer, 20);
	uint32_t myTimNum = numTimers++;
	timers[myTimNum] = mainTimer;
	
	GPIOD->BSRR |= GPIO_BSRR_BR_15;
	
	while(1) {
		//float x_reading = Lis3ReadAxis('x');
		//printf("Combined X axis movement in mg: %.1f and  \n", x_reading);

		if(SHOULD_READ == 1){
			GPIOD->BSRR |= GPIO_BSRR_BS_15;
			

			while(!Timer_IsElapsed(myTimNum)) {
				//we should yield to other tasks here ...	
				__WFI(); // Let's sleep we can be interrupted but will wait
			}
			
			int8_t today_temp = Lis3ReadTemp();
			printf("Temp is %dC Degrees Celsius \n", today_temp);
			
			GPIOD->BSRR |= GPIO_BSRR_BR_15;
			SHOULD_READ = 0;


			//AdcReadChannel(16);
		}

		
	}
	return 0;
}

/*
 * Configure timer 3 so that we can use it for some delays
 */
static void ConfigureTim7(void){
	//enable clock for timer 7
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

	// set a prescaler so we get 1KHz
	//Required freq = CLK / (PSC + 1)
	TIM7->PSC = 9999;

	/* So, this will generate the 1ms delay */
	TIM7->ARR = 47999;

	//Enable timer to generate update events
	TIM7->EGR |= TIM_EGR_UG;

	/* Enable the Interrupt */
	TIM7->DIER |= TIM_DIER_UIE;

	/* Clear the Interrupt Status */
	TIM7->SR &= ~TIM_SR_UIF;

	/* Enable NVIC Interrupt for Timer 6 */
	NVIC_EnableIRQ(TIM7_IRQn);

	/* Finally enable TIM3 module */
	TIM7->CR1 = (1 << 0);
}

void TIM7_IRQHandler(void) {
	//check if UIF flag is set
	if (TIM7->SR & TIM_SR_UIF) {
		// Clear Interrupt Flag
		TIM7->SR &= ~(TIM_SR_UIF);
	}
	SHOULD_READ = 1;
}



