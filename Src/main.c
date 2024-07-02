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
#include "../drivers/Inc/32f407_spi.h"

//for print f
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define BLUE_LED_PORT GPIOD
#define BLUE_LED_PIN 15


//Globals
SPI_Handle_t SPI2Handle;
uint8_t SPI_Actual_RX;

uint8_t dummy_read = 0xAA;


void SPI_2_Init();



void delay(void){
	for(uint32_t i = 0; i < 500000/2; i ++);
}



int main(void)
{
	printf("System booted with Default clock\n");

	SPI_2_Init();

	/*SPI Read Write*/

	uint8_t who_am_address = 0x0D | ((uint8_t)0x80);

	uint8_t dummy_write = 0x00;
	uint8_t who_am_i_data = 0 ;

	printf("Initialized SPI and reading Address: %#X for ID, Current Value: %#X \n", who_am_address, who_am_i_data);

	//Drop PE3 to select spi 1
	GPIOE->BSRR |= GPIO_BSRR_BR_3;
	printf("Enabled SPI1 by making PE3 low, GPIOE ODR: %#04X \n", GPIOE->ODR);
	SPI_SendData(&SPI2Handle,  &who_am_address, 1);

	printf("Ready for RX now going to probe MEM with a dummy write \n");
	SPI_SendData(&SPI2Handle,  &dummy_write, 1);
	printf("After Doing Dummy Write \n");

	printf("Sent data now setting receive Current value of Dummy Read: %#X \n", dummy_read);
	SPI_ReceiveData(&SPI2Handle, &dummy_read, 1);

	/*End SPI Read Write*/

	

	delay();

	//Turn on Blue LED with pure CMSIS
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER15_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_1;

	delay();

	printf("GPIO D setup and now will turn on blue LED \n");



	while(1) {

	GPIOD->BSRR |= GPIO_BSRR_BS_15;

	printf("Blue LED is now ON\n");

	delay();
	delay();
	delay();
	


	printf("GPIO D setup will turn off blue LED \n");

	GPIOD->BSRR |= GPIO_BSRR_BR_15;

	printf("Blue LED is now OFF \n");

	delay();
	delay();
	delay();
	}
	return 0;
}




void SPI_2_Init() {

	uint32_t altfn_reg;

	// GPIO B for SPI 2
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	//MOSI
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_7;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR7_0;
	//get this pins altfn reg
	altfn_reg = 7;
	GPIOA->AFR[0] |= 5 << (altfn_reg * 4);
	

	//MISO
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_6;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6_0;
	//get this pins altfn reg
	altfn_reg = 6;
	GPIOA->AFR[0] |= 5 << (altfn_reg * 4);

	//SCLK
	GPIOA->MODER |= GPIO_MODER_MODER5_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_0;
	//get this pins altfn reg
	altfn_reg = 5;
	GPIOA->AFR[0] |= 5 << (altfn_reg * 4);

	//NSS
	GPIOE->MODER |= GPIO_MODER_MODER3_0;
	GPIOE->OTYPER &= ~(1 << GPIO_OTYPER_OT3_Pos);
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR3_0;
	
	//SET PE3 to disable SPI1
	GPIOE->BSRR |= GPIO_BSRR_BS_3;
	printf("GPIOE ODR: %#X \n", GPIOE->ODR);


	

	printf("SPI 1 to MEMS ACC enabling \n");
	
	//SPI_DeInit(SPI1);

	SPI2Handle.pSPIx = SPI1;
	SPI2Handle.SPIConfig.SPI_BUSConfig = SPI_BUS_CONFIG_FD;
	SPI2Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI2Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV16;
	SPI2Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI2Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI2Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI2Handle.SPIConfig.SPI_SSM = SPI_SSM_EN; // software slave management for NSS pin
	SPI2Handle.SPIConfig.SPI_MSBFIRST = ENABLE;

	SPI_Init(&SPI2Handle);


	printf("SPI 1 to MEMS ACC enabled \n");

}

void SPI1_IRQHandler(){ 
	printf("IRQ Called \n");
	SPI_IRQHandling(&SPI2Handle);
}


//application callback
void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEv){
	// weak implementation for Application to override
	switch (AppEv) {
	case SPI_EVENT_TX_CMPLT:
		printf("We Handled SPI EVENT TX: %d \n", AppEv);
		break;
	case SPI_EVENT_RX_CMPLT:
		//if(SPI_Actual_RX){
		printf("We Handled SPI EVENT RX: %d \n", AppEv);
		printf("This is Dummy Read after RX _CPLT:  %X \n", (uint16_t)dummy_read);
		//}
		break;
	case SPI_EVENT_OVR_ERR:
		printf("We Handled SPI EVENT OVR: %d \n", AppEv);
		break;
	}


}